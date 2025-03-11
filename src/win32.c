#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#ifdef BJ_FEATURE_WIN32

#include <windows.h>

#include "system_t.h"
#include "window_t.h"

typedef struct {
    bj_system_backend fns;
    HMODULE           p_instance;
} win32_backend;

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

    const uint32_t client_x      = x;
    const uint32_t client_y      = y;
    const uint32_t client_width  = width;
    const uint32_t client_height = height;

    const uint32_t window_x      = client_x + border_rect.left;
    const uint32_t window_y      = client_y + border_rect.right;
    const uint32_t window_width  = client_width + border_rect.right - border_rect.left;
    const uint32_t window_height = client_height + border_rect.bottom - border_rect.top;

    HWND p_handle = CreateWindowExA(
        window_ex_style, "banjo_window_class", p_title,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, p_win32->p_instance, 0
    );

    if(p_handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    

    bj_window* window = bj_malloc(sizeof(bj_window));
    window->p_button_event = 0;
    window->p_cursor_event = 0;
    window->p_enter_event = 0;
    window->p_key_event = 0;
    window->must_close = true;
    return window;
}

static void win32_window_del(
    bj_system_backend* p_backend,
    bj_window* p_window
) {
    (void)p_backend;

    bj_free(p_window);
}

static void win32_dispose_backend(
    bj_system_backend* p_backend,
    bj_error** p_error
) {
    (void)p_error;

    bj_free(p_backend);
}

static void win32_window_poll(
    bj_system_backend* p_backend
) {
    bj_free((win32_backend*)p_backend);
}

static LRESULT CALLBACK win32_process_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    return 0;
}

static bj_system_backend* win32_init_backend(
    bj_error** p_error
) {
    win32_backend* p_win32 = bj_malloc(sizeof(win32_backend));

    HINSTANCE p_instance = GetModuleHandleA(0);

    HICON icon = LoadIcon(p_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    bj_memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    // wc.lpszMenuName = 0;
    wc.hInstance = p_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = "banjo_window_class";

    if(!RegisterClassA(&wc)) {
        const char* p_msg = "Window Registration Failed";
        MessageBoxA(0, p_msg, "Error", MB_ICONEXCLAMATION | MB_OK);
        bj_set_error(p_error, BJ_ERROR_SYSTEM, p_msg);
        return 0;

    }

    //
    const uint32_t window_style  = WS_OVERLAPPED  | WS_SYSMENU     | WS_CAPTION
                                 | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
    const uint32_t window_ex_style = WS_EX_APPWINDOW;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    const uint32_t client_x      = 400;
    const uint32_t client_y      = 400;
    const uint32_t client_width  = 400;
    const uint32_t client_height = 400;

    const uint32_t window_x      = client_x + border_rect.left;
    const uint32_t window_y      = client_y + border_rect.right;
    const uint32_t window_width  = client_width + border_rect.right - border_rect.left;
    const uint32_t window_height = client_height + border_rect.bottom - border_rect.top;

    HWND p_handle = CreateWindowExA(
        window_ex_style, "banjo_window_class", "Hello",
        window_style, window_x, window_y, window_width, window_height,
        0, 0, p_instance, 0
    );

    if(p_handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    win32_backend win32 = {
        .p_instance = p_instance,
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