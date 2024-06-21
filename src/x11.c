#include <banjo/error.h>
#include <banjo/memory.h>
#include <banjo/window.h>
#include <banjo/log.h>

#include <X11/Xlib.h>

#include "window_backend.h"

#define X11_CANNOT_OPEN_DISPLAY 0x00010000

struct bj_window_backend_data {
    Display* display;
};

struct bj_window_t {
    Window handle;
};

static bj_window* x11_create_window(
    bj_window_backend_data* p_data,
    const char* p_title,
    u16 x,
    u16 y,
    u16 width,
    u16 height
) {
    int screen = DefaultScreen(p_data->display);
    Window root_window = RootWindow(p_data->display, screen);


    XSetWindowAttributes attributes;
    attributes.background_pixel = WhitePixel(p_data->display, screen);
    attributes.border_pixel     = BlackPixel(p_data->display, screen);
    attributes.event_mask       = ButtonPress;

    bj_window window = { 
        .handle = XCreateWindow(
            p_data->display, root_window,
            x, y, width, height, 1,
            DefaultDepth(p_data->display, screen),
            InputOutput,
            DefaultVisual(p_data->display, screen),
            CWBackPixel | CWBorderPixel | CWEventMask, &attributes
        )
    };

    XMapWindow(p_data->display, window.handle);
    XSync(p_data->display, 0);

    // Check errors
    
    bj_window* p_window = bj_malloc(sizeof(bj_window));
    bj_memcpy(p_window, &window, sizeof(bj_window));

    return p_window;
}

static void x11_delete_window(
    bj_window_backend_data* p_data,
    bj_window* p_window
) {
    XDestroyWindow(p_data->display, p_window->handle);
    bj_free(p_window);
}

static void x11_dispose_backend(
    bj_window_backend* p_backend,
    bj_error** p_error
) {
    XCloseDisplay(p_backend->p_data->display);
    bj_free(p_backend->p_data);
    bj_free(p_backend);
}

static bj_window_backend* x11_init_backend(
    bj_error** p_error
) {
    Display* display = XOpenDisplay(0);
    if(display == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE | X11_CANNOT_OPEN_DISPLAY, "DAFUQ");
        return 0;
    }

    bj_window_backend* p_backend = bj_malloc(sizeof(bj_window_backend));
    p_backend->dispose = x11_dispose_backend;
    p_backend->create_window = x11_create_window;
    p_backend->delete_window = x11_delete_window;

    p_backend->p_data = bj_malloc(sizeof(bj_window_backend_data));
    p_backend->p_data->display = display;
    return p_backend;
}

bj_window_backend_create_info x11_backend_create_info = {
    .name = "x11",
    .create = x11_init_backend,
};

