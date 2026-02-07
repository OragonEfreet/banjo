

#ifdef BJ_CONFIG_WIN32_BACKEND

#include <banjo/assert.h>
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/renderer.h>

#include <bitmap.h>
#include <check.h>
#include <renderer.h>
#include <video_layer.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <windowsx.h>

#include <window.h>

#define WIN32_WINDOWCLASS_NAME ("banjo_window_class")

struct {
    HINSTANCE hInstance;
} win32;

struct bj_renderer_data {
    struct bj_bitmap framebuffer;
    HDC              hdc;
    HDC              fbdc;
    HBITMAP          fbmp;
};

struct win32_window {
    struct bj_window common;
    HWND             handle;
    int              cursor_in_window;
    HDC              hdc;
    HDC              fbdc;
    HBITMAP          fbmp;
};

static struct bj_window* win32_window_new(
    const char*       title,
    uint16_t          x,
    uint16_t          y,
    uint16_t          width,
    uint16_t          height,
    uint8_t           flags,
    struct bj_error** error
) {
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
        window_ex_style, WIN32_WINDOWCLASS_NAME, title, window_style,
        window_x, window_y, window_width, window_height,
        NULL, NULL, win32.hInstance, 0
    );

    if (!hwnd) {
        bj_set_error_fmt(error, BJ_ERROR_VIDEO,
                         "Failed to create window (error %lu)", GetLastError());
        return 0;
    }

    struct win32_window window = {
        .common = {
            .flags = flags,
        },
        .handle = hwnd,
        .hdc = GetDC(hwnd),
        .cursor_in_window = BJ_FALSE,
    };

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    struct win32_window* window_res = bj_malloc(sizeof(struct win32_window));
    if (window_res == 0) {
        DestroyWindow(hwnd);
        bj_set_error(error, BJ_ERROR_VIDEO, "Failed to allocate window structure");
        return 0;
    }
    bj_memcpy(window_res, &window, sizeof(struct win32_window));

    SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window_res);

    return (struct bj_window*)window_res;
}

static void win32_delete_window_framebuffer(
    struct win32_window* window
) {
    if (window->fbdc) {
        DeleteDC(window->fbdc);
    }
    if (window->fbmp) {
        DeleteObject(window->fbmp);
    }
}

static void win32_window_del(
    struct bj_window* abstract_window
) {
    struct win32_window* window = (struct win32_window*)abstract_window;
    win32_delete_window_framebuffer(window);
    ReleaseDC(window->handle, window->hdc);
    DestroyWindow(window->handle);
    bj_free(window);
}



static int win32_get_window_size(
    const struct bj_window* abstract_window,
    int* width,
    int* height
) {
    struct win32_window* window = (struct win32_window*)abstract_window;
    RECT rect;

    if (GetClientRect(window->handle, &rect)) {
        *width = rect.right;
        *height = rect.bottom;
        return 1;
    }

    return 0;
}

static void win32_end_video(
    struct bj_error** error
) {
    (void)error;

    if(!UnregisterClassA(WIN32_WINDOWCLASS_NAME, win32.hInstance)) {
        bj_set_error(error, BJ_ERROR_DISPOSE, "Failed to unregister window class");
    }
    win32.hInstance = 0;   
}

static void win32_window_poll(
    void
) {
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

static enum bj_key vk_to_bj_key(WPARAM wParam, LPARAM lParam) {
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

    return (enum bj_key)wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    struct win32_window* window = (struct win32_window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    switch (uMsg) {

        case WM_CLOSE:
            bj_set_window_should_close((struct bj_window*)window);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_ERASEBKGND:
            return 1; // We handle it

        // case WM_CHAR: bj_trace("WM_CHAR"); break;
        // case WM_UNICHAR: bj_trace("WM_UNICHAR"); break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP: {

            const int action = (HIWORD(lParam) & KF_UP) ? BJ_RELEASE : BJ_PRESS;
            const int scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xFF));
            enum bj_key keycode = vk_to_bj_key(wParam, lParam);

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

            bj_push_key_event((struct bj_window*)window, action, keycode, scancode);
            
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            bj_push_button_event((struct bj_window*)window, 
                BJ_BUTTON_LEFT,
                uMsg == WM_LBUTTONDOWN ? BJ_PRESS : BJ_RELEASE,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            bj_push_button_event((struct bj_window*)window, 
                BJ_BUTTON_MIDDLE,
                uMsg == WM_MBUTTONDOWN ? BJ_PRESS : BJ_RELEASE,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_MOUSEMOVE: {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if(!window->cursor_in_window) {
                window->cursor_in_window = 1;
                bj_push_enter_event((struct bj_window*)window, 1, x, y);
            }
            TRACKMOUSEEVENT tme = {0};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            bj_push_cursor_event((struct bj_window*)window, x, y);
            break;
        }

        case WM_MOUSELEAVE: {
            window->cursor_in_window = 0;
            bj_push_enter_event((struct bj_window*)window, 0, 0, 0);
            break;
        }

        case WM_MOUSEWHEEL:
            bj_push_button_event((struct bj_window*)window, 
                GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? BJ_BUTTON_UP : BJ_BUTTON_DOWN,
                BJ_PRESS,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            break;

        case WM_RBUTTONDOWN:

        case WM_RBUTTONUP:
            bj_push_button_event((struct bj_window*)window, 
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


static bj_bool win32_renderer_configure(
    struct bj_renderer* renderer,
    struct bj_window*   abstract_window,
    struct bj_error**   error
) {
    struct win32_window* window = (struct win32_window*)abstract_window;
    struct bj_renderer_data* data = renderer->data;

    int width  = 0;
    int height = 0;

    data->hdc = window->hdc;

    if (!win32_get_window_size((const struct bj_window*)window, &width, &height)) {
        bj_set_error(error, BJ_ERROR_VIDEO, "Cannot get window dimensions");
        return BJ_FALSE;
    }

    if (data->fbdc) {
        DeleteDC(data->fbdc);
    }
    if (data->fbmp) {
        DeleteObject(data->fbmp);
    }

    const size_t info_size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    LPBITMAPINFO bmp_info = bj_malloc(info_size);
    bj_memset(bmp_info, 0, info_size);
    bmp_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    HBITMAP h_bmp = CreateCompatibleBitmap(data->hdc, 1, 1);
    GetDIBits(data->hdc, h_bmp, 0, 0, NULL, bmp_info, DIB_RGB_COLORS);
    GetDIBits(data->hdc, h_bmp, 0, 0, NULL, bmp_info, DIB_RGB_COLORS);
    DeleteObject(h_bmp);

    enum bj_pixel_mode pixel_mode = BJ_PIXEL_MODE_UNKNOWN;
    if (bmp_info->bmiHeader.biCompression == BI_BITFIELDS) {
        int bpp = bmp_info->bmiHeader.biPlanes * bmp_info->bmiHeader.biBitCount;
        int32_t* masks = (int32_t*)((int8_t*)bmp_info + bmp_info->bmiHeader.biSize);
        pixel_mode = bj_compute_pixel_mode(bpp, masks[0], masks[1], masks[2]);
    }
    if (pixel_mode == BJ_PIXEL_MODE_UNKNOWN) {
        pixel_mode = BJ_PIXEL_MODE_XRGB8888;
        bj_memset(bmp_info, 0, info_size);
        bmp_info->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmp_info->bmiHeader.biPlanes      = 1;
        bmp_info->bmiHeader.biBitCount    = 32;
        bmp_info->bmiHeader.biCompression = BI_RGB;
    }

    const size_t stride = bj_compute_bitmap_stride(width, pixel_mode);

    if (stride == 0) {
        bj_set_error(error, BJ_ERROR_VIDEO, "Invalid window pixel format");
        bj_free(bmp_info);
        return BJ_FALSE;
    }

    bmp_info->bmiHeader.biWidth = width;
    bmp_info->bmiHeader.biHeight = -height;
    bmp_info->bmiHeader.biSizeImage = (DWORD)height * stride;

    void* pixels = 0;
    data->fbdc = CreateCompatibleDC(data->hdc);
    data->fbmp = CreateDIBSection(data->hdc, bmp_info, DIB_RGB_COLORS, &pixels, NULL, 0);

    bj_free(bmp_info);

    if (!data->fbmp) {
        bj_set_error(error, BJ_ERROR_VIDEO, "Cannot create DIB section");
        return BJ_FALSE;
    }

    SelectObject(data->fbdc, data->fbmp);

    bj_assign_bitmap(&data->framebuffer, pixels, width, height, pixel_mode, stride);
    return BJ_TRUE;
}

static struct bj_bitmap* win32_renderer_get_framebuffer(
    struct bj_renderer* renderer
) {
    return &renderer->data->framebuffer;
}

static void win32_renderer_present(
    struct bj_renderer* renderer,
    struct bj_window* abstract_window
) {
    struct win32_window* window = (struct win32_window*)abstract_window;

    int width = 0;
    int height = 0;
    if (win32_get_window_size(abstract_window, &width, &height)) {
        BitBlt(renderer->data->hdc, 0, 0, width, height, renderer->data->fbdc, 0, 0, SRCCOPY);
    }
}

static struct bj_renderer* win32_create_renderer(
    enum bj_renderer_type  type,
    struct bj_error**      error
) {
    (void)type;
    struct bj_renderer* renderer = bj_calloc(sizeof(struct bj_renderer));
    if (renderer == 0) {
        bj_set_error(error, BJ_ERROR_VIDEO, "Failed to allocate renderer");
        return 0;
    }
    renderer->data = bj_calloc(sizeof(struct bj_renderer_data));
    if (renderer->data == 0) {
        bj_free(renderer);
        bj_set_error(error, BJ_ERROR_VIDEO, "Failed to allocate renderer data");
        return 0;
    }

    renderer->configure       = win32_renderer_configure;
    renderer->get_framebuffer = win32_renderer_get_framebuffer;
    renderer->present         = win32_renderer_present;

    return renderer;
}

static void win32_destroy_renderer(
    struct bj_renderer* renderer
) {
    bj_check(renderer);

    struct bj_renderer_data* data = &renderer->data;

    // Clean up the framebuffer bitmap internals
    bj_reset_bitmap(&data->framebuffer);

    if (data->fbdc) {
        DeleteDC(data->fbdc);
    }
    if (data->fbmp) {
        DeleteObject(data->fbmp);
    }

    bj_free(renderer->data);
    bj_free(renderer);
}

static struct bj_video_layer* win32_init_video(
    struct bj_video_layer* layer,
    struct bj_error** error
) {

    win32.hInstance = GetModuleHandleA(0);

    if (!RegisterClassA(&(WNDCLASSA){
        .lpfnWndProc   = WindowProc,
        .hInstance     = win32.hInstance,
        .lpszClassName = WIN32_WINDOWCLASS_NAME,
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .hCursor       = LoadCursor(NULL, IDC_ARROW),
    })) {
        bj_set_error(error, BJ_ERROR_INITIALIZE, "Failed to register window class");
        return BJ_FALSE;
    }

    layer->end                       = win32_end_video;
    layer->create_window             = win32_window_new;
    layer->delete_window             = win32_window_del;
    layer->poll_events               = win32_window_poll;
    layer->get_window_size           = win32_get_window_size;
    layer->create_renderer           = win32_create_renderer;
    layer->destroy_renderer          = win32_destroy_renderer;
    return BJ_TRUE;
}

struct bj_video_layer_create_info win32_video_layer_info = {
    .name   = "win32",
    .create = win32_init_video,
};

#endif
