#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "config.h"

#if BJ_HAS_FEATURE(WIN32)

#include <windows.h>
#include <windowsx.h>

#include "system_t.h"
#include "window_t.h"

#define WIN32_WINDOWCLASS_NAME ("banjo_window_class")

typedef struct {
    bj_system_backend fns;
    HINSTANCE         p_instance;
} win32_backend;

typedef struct {
    struct bj_window_t common;
    HWND               handle;
    int                cursor_in_window;
} win32_window;

static bj_window* win32_window_new(
    bj_system_backend* p_backend,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
) {
    win32_backend* p_win32 = (win32_backend*)p_backend;

    const uint32_t window_style  = WS_OVERLAPPED  | WS_SYSMENU     | WS_CAPTION
                                 | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
    const uint32_t window_ex_style = WS_EX_APPWINDOW;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);
    const int window_x      = x + border_rect.left;
    const int window_y      = y + border_rect.right;
    const int window_width  = width + border_rect.right - border_rect.left;
    const int window_height = height + border_rect.bottom - border_rect.top;

    // Create the window
    HWND hwnd = CreateWindowExA(
        window_ex_style, WIN32_WINDOWCLASS_NAME, p_title, window_style,
        window_x, window_y, window_width, window_height,
        NULL, NULL, p_win32->p_instance, "Hello"
    );

    if (!hwnd) {
        return 0;
    }

    win32_window window = { 
        .common = {
            .must_close = false,
        },
        .handle = hwnd,
        .cursor_in_window = false,
    };

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    win32_window* p_window = bj_malloc(sizeof(win32_window));
    bj_memcpy(p_window, &window, sizeof(win32_window));

    SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)p_window);

    return (bj_window*)p_window;
}

static void win32_window_del(
    bj_system_backend* p_backend,
    bj_window* p_abstract_window
) {
    (void)p_backend;
    win32_window* p_window = (win32_window*)p_abstract_window;
    DestroyWindow(p_window->handle);
    bj_free(p_window);
}

static void win32_dispose_backend(
    bj_system_backend* p_backend,
    bj_error** p_error
) {
    (void)p_error;

    if(!UnregisterClassA(
        WIN32_WINDOWCLASS_NAME,
        ((win32_backend*)p_backend)->p_instance)
    ) {
        bj_set_error(p_error, BJ_ERROR_DISPOSE, "Failed to unregister window class");
    }

    bj_free(p_backend);
}

static void win32_window_poll(
    bj_system_backend* p_backend
) {
    (void)p_backend;
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    win32_window* p_window = (win32_window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    switch (uMsg) {

        case WM_CLOSE:
            bj_window_set_should_close((bj_window*)p_window);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_ERASEBKGND:
            return 1; // We handle it

        // TODO
        // case WM_CHAR: bj_trace("WM_CHAR"); break;
        // case WM_UNICHAR: bj_trace("WM_UNICHAR"); break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP: {
            bj_window_input_key(
                (bj_window*)p_window,
                (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) ? BJ_PRESS : BJ_RELEASE,
                (lParam >> 16) & 0xFF
            );
            
            break;
        }


        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            bj_window_input_button((bj_window*)p_window, 
                BJ_BUTTON_LEFT,
                uMsg == WM_LBUTTONDOWN ? BJ_PRESS : BJ_RELEASE,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_MBUTTONDOWN:

        case WM_MBUTTONUP:
            bj_window_input_button((bj_window*)p_window, 
                BJ_BUTTON_MIDDLE,
                uMsg == WM_MBUTTONDOWN ? BJ_PRESS : BJ_RELEASE,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_MOUSEMOVE: {

            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if(!p_window->cursor_in_window) {
                p_window->cursor_in_window = 1;
                bj_window_input_enter((bj_window*)p_window, 1, x, y);
            }
            TRACKMOUSEEVENT tme = {0};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            bj_window_input_cursor((bj_window*)p_window, x, y);
            break;
        }

        case WM_MOUSELEAVE: {
            p_window->cursor_in_window = 0;
            bj_window_input_enter((bj_window*)p_window, 0, 0, 0);
            break;
        }

        case WM_MOUSEWHEEL:
            bj_window_input_button((bj_window*)p_window, 
                GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? BJ_BUTTON_UP : BJ_BUTTON_DOWN,
                BJ_PRESS,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_RBUTTONDOWN:

        case WM_RBUTTONUP:
            bj_window_input_button((bj_window*)p_window, 
                BJ_BUTTON_RIGHT,
                uMsg == WM_RBUTTONDOWN ? BJ_PRESS : BJ_RELEASE,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_SIZE:           bj_trace("WM_SIZE");        break;

        default:
            return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

static bj_system_backend* win32_init_backend(
    bj_error** p_error
) {
    HINSTANCE hInstance = GetModuleHandleA(0);

    if (!RegisterClassA(&(WNDCLASSA){
        .lpfnWndProc   = WindowProc,
        .hInstance     = hInstance,
        .lpszClassName = WIN32_WINDOWCLASS_NAME,
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .hCursor       = LoadCursor(NULL, IDC_ARROW),
    })) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "Failed to register window class");
        return 0;
    }

    win32_backend win32 = {
        .p_instance = hInstance,
        .fns = {
            .dispose           = win32_dispose_backend,
            .create_window     = win32_window_new,
            .delete_window     = win32_window_del,
            .poll_events       = win32_window_poll,
        },
    };
    return bj_memcpy(bj_malloc(sizeof(win32_backend)), &win32, sizeof(win32_backend));
}

bj_system_backend_create_info win32_backend_create_info = {
    .name = "Win32",
    .create = win32_init_backend,
};

#endif
