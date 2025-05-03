#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "config.h"

#if BJ_HAS_FEATURE(WIN32)

#include <windows.h>
#include <windowsx.h>

#include "system_t.h"
#include "window_t.h"

#include <assert.h>

#define WIN32_WINDOWCLASS_NAME ("banjo_window_class")

typedef struct {
    bj_system_backend fns;
    HINSTANCE         p_instance;
} win32_backend;

typedef struct {
    struct bj_window_t common;
    HWND               handle;
    int                cursor_in_window;
    HDC                hdc;
    HDC                fbdc;
    HBITMAP            fbmp;
} win32_window;

static bj_window* win32_window_new(
    bj_system_backend* p_backend,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
) {
    win32_backend* p_win32 = (win32_backend*)p_backend;

    const uint32_t window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
                                 //| WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
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
        NULL, NULL, p_win32->p_instance, 0
    );

    if (!hwnd) {
        return 0;
    }

    win32_window window = { 
        .common = {
            .flags = flags,
        },
        .handle = hwnd,
        .hdc = GetDC(hwnd),
        .cursor_in_window = false,
    };

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    win32_window* p_window = bj_malloc(sizeof(win32_window));
    bj_memcpy(p_window, &window, sizeof(win32_window));

    SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)p_window);

    return (bj_window*)p_window;
}

static void win32_delete_window_framebuffer(
    win32_window* p_window
) {
    if (p_window->fbdc) {
        DeleteDC(p_window->fbdc);
    }
    if (p_window->fbmp) {
        DeleteObject(p_window->fbmp);
    }
}

static void win32_window_del(
    bj_system_backend* p_backend,
    bj_window* p_abstract_window
) {
    (void)p_backend;
    win32_window* p_window = (win32_window*)p_abstract_window;
    win32_delete_window_framebuffer(p_window);
    ReleaseDC(p_window->handle, p_window->hdc);
    DestroyWindow(p_window->handle);
    bj_free(p_window);
}

bj_bitmap* win32_create_window_framebuffer(
    bj_system_backend* p_backend,
    const bj_window* p_abstract_window,
    bj_error** p_error
) {
    win32_window* p_window = (win32_window*)p_abstract_window;

    int width = 0;
    int height = 0;
    if (!win32_get_window_size(p_backend, p_window, &width, &height)) {
        bj_set_error(p_error, BJ_ERROR_BACKEND, "Cannot get window dimension");
        return 0;
    }

    win32_delete_window_framebuffer(p_window);
    
    const size_t info_size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    LPBITMAPINFO p_bmp_info = bj_malloc(info_size);
    bj_memset(p_bmp_info, 0, info_size);
    p_bmp_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    HBITMAP h_bmp = CreateCompatibleBitmap(p_window->hdc, 1, 1);
    GetDIBits(p_window->hdc, h_bmp, 0, 0, NULL, p_bmp_info, DIB_RGB_COLORS);
    GetDIBits(p_window->hdc, h_bmp, 0, 0, NULL, p_bmp_info, DIB_RGB_COLORS);
    DeleteObject(h_bmp);

    bj_pixel_mode pixel_mode = BJ_PIXEL_MODE_UNKNOWN;
    if (p_bmp_info->bmiHeader.biCompression == BI_BITFIELDS) {
        int bpp = p_bmp_info->bmiHeader.biPlanes * p_bmp_info->bmiHeader.biBitCount;
        int32_t* masks = (int32_t*)((int8_t*)p_bmp_info + p_bmp_info->bmiHeader.biSize);
        pixel_mode = bj_compute_pixel_mode(bpp, masks[0], masks[1], masks[2]);
    }
    if (pixel_mode == BJ_PIXEL_MODE_UNKNOWN) {
        pixel_mode = BJ_PIXEL_MODE_XRGB8888;
        bj_memset(p_bmp_info, 0, info_size);
        p_bmp_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        p_bmp_info->bmiHeader.biPlanes = 1;
        p_bmp_info->bmiHeader.biBitCount = 32;
        p_bmp_info->bmiHeader.biCompression = BI_RGB;
    }

    const size_t stride = bj_compute_bitmap_stride(width, pixel_mode);

    if(stride == 0) {
        bj_set_error(p_error, BJ_ERROR_BACKEND, "Invalid window pixel format");
        bj_free(p_bmp_info);
        return 0;
    }

    p_bmp_info->bmiHeader.biWidth = width;
    p_bmp_info->bmiHeader.biHeight = -height;
    p_bmp_info->bmiHeader.biSizeImage = (DWORD)height * stride;

    void* pixels = 0;
    p_window->fbdc = CreateCompatibleDC(p_window->hdc);
    p_window->fbmp = CreateDIBSection(p_window->hdc, p_bmp_info, DIB_RGB_COLORS, &pixels, NULL, 0);

    bj_free(p_bmp_info);

    if (!p_window->fbmp) {
        bj_set_error(p_error, BJ_ERROR_BACKEND, "Cannot create DIB section");
        return 0;
    }
    SelectObject(p_window->fbdc, p_window->fbmp);

    return bj_bitmap_new_from_pixels(pixels, width, height, pixel_mode, stride);
}

static int win32_get_window_size(
    bj_system_backend* p_backend,
    const bj_window* p_abstract_window,
    int* width,
    int* height
) {
    (void)p_backend;
    win32_window* p_window = (win32_window*)p_abstract_window;
    HWND handle = p_window->handle;
    RECT rect;

    if (GetClientRect(p_window->handle, &rect)) {
        *width = rect.right;
        *height = rect.bottom;
        return 1;
    }

    return 0;
}

static void win32_flush_window_framebuffer(
    bj_system_backend* p_backend,
    const bj_window*   p_abstract_window
) {
    win32_window* p_window = (win32_window*)p_abstract_window;
    assert(p_window->common.p_framebuffer != 0);

    int width = 0;
    int height = 0;
    if (win32_get_window_size(p_backend, p_window, &width, &height)) {
        BitBlt(p_window->hdc, 0, 0, width, height, p_window->fbdc, 0, 0, SRCCOPY);
    }
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

static bj_key vk_to_bj_key(WPARAM wParam, LPARAM lParam) {
    if(wParam < 0 || wParam >= 0xFF) {
        return BJ_KEY_UNKNOWN;
    }

    switch(wParam) {
        case BJ_KEY_MENU:
            return HIWORD(lParam) & KF_EXTENDED ? BJ_KEY_RMENU : BJ_KEY_LMENU;
        case BJ_KEY_SHIFT:
            return HIWORD(lParam) & KF_EXTENDED ? BJ_KEY_RSHIFT : BJ_KEY_LSHIFT;
        case BJ_KEY_CONTROL:
            return HIWORD(lParam) & KF_EXTENDED ? BJ_KEY_RCONTROL : BJ_KEY_LCONTROL;
        default:
            break;
    }

    return (bj_key)wParam;
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
            bj_key keycode = vk_to_bj_key(wParam, lParam);

            if(keycode == BJ_KEY_LCONTROL) {
                MSG next;
                const DWORD time = GetMessageTime();
                if(PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE)) {
                    if(next.message == WM_KEYDOWN
                    || next.message == WM_SYSKEYDOWN
                    || next.message == WM_KEYUP
                    || next.message == WM_SYSKEYUP) {
                        if(vk_to_bj_key(next.wParam, next.lParam) == BJ_KEY_RMENU && next.time == time) {
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

        case WM_SIZE:           /*bj_trace("WM_SIZE");*/        break;

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
            .dispose                   = win32_dispose_backend,
            .create_window             = win32_window_new,
            .delete_window             = win32_window_del,
            .poll_events               = win32_window_poll,
            .get_window_size           = win32_get_window_size,
            .create_window_framebuffer = win32_create_window_framebuffer,
            .flush_window_framebuffer  = win32_flush_window_framebuffer,
        },
    };
    return bj_memcpy(bj_malloc(sizeof(win32_backend)), &win32, sizeof(win32_backend));
}

bj_system_backend_create_info win32_backend_create_info = {
    .name = "Win32",
    .create = win32_init_backend,
};

#endif
