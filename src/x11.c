#include <banjo/error.h>
#include <banjo/memory.h>
#include <banjo/window.h>
#include <banjo/log.h>

#include <X11/Xlib.h>

#include "window_backend.h"

#define X11_CANNOT_OPEN_DISPLAY 0x00010000

typedef struct {
    bj_window_backend fns;
    Display* display;
    int screen;
} x11_backend;

struct bj_window_t {
    Window handle;
};

static bj_window* x11_create_window(
    bj_window_backend* p_backend,
    const char* p_title,
    u16 x,
    u16 y,
    u16 width,
    u16 height
) {
    x11_backend* p_x11 = (x11_backend*)p_backend;
    Window root_window = RootWindow(p_x11->display, p_x11->screen);

    XSetWindowAttributes attributes;
    attributes.background_pixel = WhitePixel(p_x11->display, p_x11->screen);
    attributes.border_pixel     = BlackPixel(p_x11->display, p_x11->screen);
    attributes.event_mask       = ButtonPress;

    bj_window window = { 
        .handle = XCreateWindow(
            p_x11->display, root_window,
            x, y, width, height, 1,
            DefaultDepth(p_x11->display, p_x11->screen),
            InputOutput,
            DefaultVisual(p_x11->display, p_x11->screen),
            CWBackPixel | CWBorderPixel | CWEventMask, &attributes
        )
    };

    XMapWindow(p_x11->display, window.handle);
    XSync(p_x11->display, 0);

    // Check errors
    
    bj_window* p_window = bj_malloc(sizeof(bj_window));
    bj_memcpy(p_window, &window, sizeof(bj_window));

    return p_window;
}

static void x11_delete_window(
    bj_window_backend* p_backend,
    bj_window* p_window
) {
    x11_backend* p_x11 = (x11_backend*)p_backend;
    XDestroyWindow(p_x11->display, p_window->handle);
    XSync(p_x11->display, 0);
    bj_free(p_window);
}

static void x11_dispose_backend(
    bj_window_backend* p_backend,
    bj_error** p_error
) {
    x11_backend* p_x11 = (x11_backend*)p_backend;
    XCloseDisplay(p_x11->display);
    bj_free(p_backend);
}

static bj_window_backend* x11_init_backend(
    bj_error** p_error
) {
    Display* display = XOpenDisplay(0);
    if(display == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE | X11_CANNOT_OPEN_DISPLAY, "Cannot open X11 display");
        return 0;
    }

    bj_debug("X11 display connection: %d", XConnectionNumber(display));

    x11_backend* p_x11 = bj_malloc(sizeof(x11_backend));

    p_x11->fns.dispose = x11_dispose_backend;
    p_x11->fns.create_window = x11_create_window;
    p_x11->fns.delete_window = x11_delete_window;

    p_x11->display = display;
    p_x11->screen = DefaultScreen(display);
    return (bj_window_backend*)p_x11;
}

bj_window_backend_create_info x11_backend_create_info = {
    .name = "x11",
    .create = x11_init_backend,
};

