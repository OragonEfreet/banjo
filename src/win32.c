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

static int vk_to_bj_key(WPARAM wParam, LPARAM lParam) {
    switch(wParam) {
        case '0': return BJ_KEY_0;
        case '1': return BJ_KEY_1;
        case '2': return BJ_KEY_2;
        case '3': return BJ_KEY_3;
        case '4': return BJ_KEY_4;
        case '5': return BJ_KEY_5;
        case '6': return BJ_KEY_6;
        case '7': return BJ_KEY_7;
        case '8': return BJ_KEY_8;
        case '9': return BJ_KEY_9;
        case 'A': return BJ_KEY_A;
        case 'B': return BJ_KEY_B;
        case 'C': return BJ_KEY_C;
        case 'D': return BJ_KEY_D;
        case 'E': return BJ_KEY_E;
        case 'F': return BJ_KEY_F;
        case 'G': return BJ_KEY_G;
        case 'H': return BJ_KEY_H;
        case 'I': return BJ_KEY_I;
        case 'J': return BJ_KEY_J;
        case 'K': return BJ_KEY_K;
        case 'L': return BJ_KEY_L;
        case 'M': return BJ_KEY_M;
        case 'N': return BJ_KEY_N;
        case 'O': return BJ_KEY_O;
        case 'P': return BJ_KEY_P;
        case 'Q': return BJ_KEY_Q;
        case 'R': return BJ_KEY_R;
        case 'S': return BJ_KEY_S;
        case 'T': return BJ_KEY_T;
        case 'U': return BJ_KEY_U;
        case 'V': return BJ_KEY_V;
        case 'W': return BJ_KEY_W;
        case 'X': return BJ_KEY_X;
        case 'Y': return BJ_KEY_Y;
        case 'Z': return BJ_KEY_Z;
        case VK_ADD: return BJ_KEY_PAD_ADD;
        case VK_BACK: return BJ_KEY_BACKSPACE;
        case VK_CAPITAL: return BJ_KEY_CAPSLOCK;
        case VK_DECIMAL: return BJ_KEY_PAD_DECIMAL;
        case VK_DELETE: return BJ_KEY_DELETE;
        case VK_DIVIDE: return BJ_KEY_PAD_DIVIDE;
        case VK_DOWN: return BJ_KEY_DOWN;
        case VK_END: return BJ_KEY_END;
        case VK_ESCAPE: return BJ_KEY_ESCAPE;
        case VK_F10: return BJ_KEY_F10;
        case VK_F11: return BJ_KEY_F11;
        case VK_F12: return BJ_KEY_F12;
        case VK_F13: return BJ_KEY_F13;
        case VK_F14: return BJ_KEY_F14;
        case VK_F15: return BJ_KEY_F15;
        case VK_F16: return BJ_KEY_F16;
        case VK_F17: return BJ_KEY_F17;
        case VK_F18: return BJ_KEY_F18;
        case VK_F19: return BJ_KEY_F19;
        case VK_F1: return BJ_KEY_F1;
        case VK_F20: return BJ_KEY_F20;
        case VK_F21: return BJ_KEY_F21;
        case VK_F22: return BJ_KEY_F22;
        case VK_F23: return BJ_KEY_F23;
        case VK_F24: return BJ_KEY_F24;
        case VK_F2: return BJ_KEY_F2;
        case VK_F3: return BJ_KEY_F3;
        case VK_F4: return BJ_KEY_F4;
        case VK_F5: return BJ_KEY_F5;
        case VK_F6: return BJ_KEY_F6;
        case VK_F7: return BJ_KEY_F7;
        case VK_F8: return BJ_KEY_F8;
        case VK_F9: return BJ_KEY_F9;
        case VK_HOME: return BJ_KEY_HOME;
        case VK_INSERT: return BJ_KEY_INSERT;
        case VK_LCONTROL: return BJ_KEY_CONTROL_LEFT;
        case VK_LEFT: return BJ_KEY_LEFT;
        case VK_LMENU: return BJ_KEY_ALT_LEFT;
        case VK_MENU:
            return HIWORD(lParam) & KF_EXTENDED ? BJ_KEY_ALT_RIGHT : BJ_KEY_ALT_LEFT;
        case VK_LSHIFT: return BJ_KEY_SHIFT_LEFT;
        case VK_LWIN: return BJ_KEY_OS_LEFT;
        case VK_MULTIPLY: return BJ_KEY_PAD_MULTIPLY;
        case VK_NEXT: return BJ_KEY_NEXT;
        case VK_NUMLOCK: return BJ_KEY_NUMLOCK;
        case VK_NUMPAD0: return BJ_KEY_PAD_0;
        case VK_NUMPAD1: return BJ_KEY_PAD_1;
        case VK_NUMPAD2: return BJ_KEY_PAD_2;
        case VK_NUMPAD3: return BJ_KEY_PAD_3;
        case VK_NUMPAD4: return BJ_KEY_PAD_4;
        case VK_NUMPAD5: return BJ_KEY_PAD_5;
        case VK_NUMPAD6: return BJ_KEY_PAD_6;
        case VK_NUMPAD7: return BJ_KEY_PAD_7;
        case VK_NUMPAD8: return BJ_KEY_PAD_8;
        case VK_NUMPAD9: return BJ_KEY_PAD_9;
        case VK_OEM_1: return BJ_KEY_SEMICOLON;
        case VK_OEM_2: return BJ_KEY_SLASH;
        case VK_OEM_3: return BJ_KEY_GRAVE_ACCENT;
        case VK_OEM_4: return BJ_KEY_LEFT_BRACKET;
        case VK_OEM_5: return BJ_KEY_BACKSLASH;
        case VK_OEM_6: return BJ_KEY_RIGHT_BRACKET;
        case VK_OEM_7: return BJ_KEY_APOSTROPHE;
        case VK_OEM_COMMA: return BJ_KEY_COMMA;
        case VK_OEM_MINUS: return BJ_KEY_MINUS;
        case VK_OEM_PERIOD: return BJ_KEY_PERIOD;
        case VK_OEM_PLUS: return BJ_KEY_EQUAL;
        case VK_PAUSE: return BJ_KEY_PAUSE;
        case VK_PRIOR: return BJ_KEY_PRIOR;
        case VK_CONTROL:
            return HIWORD(lParam) & KF_EXTENDED ? BJ_KEY_CONTROL_RIGHT : BJ_KEY_CONTROL_LEFT;
        case VK_RCONTROL: return BJ_KEY_CONTROL_RIGHT;
        case VK_RETURN: return BJ_KEY_RETURN;
        case VK_RIGHT: return BJ_KEY_RIGHT;
        case VK_RMENU: return BJ_KEY_ALT_RIGHT;
        case VK_RSHIFT: return BJ_KEY_SHIFT_RIGHT;
        case VK_SHIFT: 
            return HIWORD(lParam) & KF_EXTENDED ? BJ_KEY_SHIFT_RIGHT : BJ_KEY_SHIFT_LEFT;
        case VK_RWIN: return BJ_KEY_OS_RIGHT;
        case VK_SCROLL: return BJ_KEY_SCROLL;
        case VK_SNAPSHOT: return BJ_KEY_PRINTSCREEN;
        case VK_SPACE: return BJ_KEY_SPACE;
        case VK_SUBTRACT: return BJ_KEY_PAD_SUBTRACT;
        case VK_TAB: return BJ_KEY_TAB;
        case VK_UP: return BJ_KEY_UP;
    }
    bj_warn("Unknown code for %x", wParam);
    return BJ_KEY_UNKNOWN;
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

            const int action = (HIWORD(lParam) & KF_UP) ? BJ_RELEASE : BJ_PRESS;
            const int scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xFF));
            int keycode = vk_to_bj_key(wParam, lParam);

            if(keycode == BJ_KEY_CONTROL_LEFT) {
                MSG next;
                const DWORD time = GetMessageTime();
                if(PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE)) {
                    if(next.message == WM_KEYDOWN
                    || next.message == WM_SYSKEYDOWN
                    || next.message == WM_KEYUP
                    || next.message == WM_SYSKEYUP) {
                        if(vk_to_bj_key(next.wParam, next.lParam) && next.time == time) {
                            break;
                        }
                    }
                }
            }

            bj_window_input_key((bj_window*)p_window, action, keycode, scancode);
            
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
