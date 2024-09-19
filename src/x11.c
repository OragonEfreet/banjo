#include <banjo/error.h>
#include <banjo/memory.h>
#include <banjo/window.h>
#include <banjo/log.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "window_backend.h"

#define X11_CANNOT_OPEN_DISPLAY 0x00010000

typedef struct {
    bj_window_backend fns;
    Display*          display;
    int               screen;
    Atom              wm_protocols;
    Atom              wm_delete_window;
    XContext          window_context;
} x11_backend;

struct bj_window_t {
    Window handle;
    bool   must_close;
};

static bj_window* x11_create_window(
    bj_window_backend* p_backend,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
) {
    x11_backend* p_x11 = (x11_backend*)p_backend;
    Window root_window = RootWindow(p_x11->display, p_x11->screen);

    XSetWindowAttributes attributes = {
        .background_pixel = BlackPixel(p_x11->display, p_x11->screen),
        .border_pixel     = BlackPixel(p_x11->display, p_x11->screen),
        .event_mask       = KeyReleaseMask | KeyPressMask,
    };

    bj_window window = { 
        .handle = XCreateWindow(
            p_x11->display, root_window,
            x, y,
            width, height, 1,
            DefaultDepth(p_x11->display, p_x11->screen),
            InputOutput,
            DefaultVisual(p_x11->display, p_x11->screen),
            CWBackPixel | CWBorderPixel | CWEventMask, &attributes
        ),
        .must_close = false,
    };

    XSetWMProtocols(
        p_x11->display, window.handle,
        &p_x11->wm_delete_window, 1
    );


    XMapWindow(p_x11->display, window.handle);
    XSync(p_x11->display, 0);

    
    bj_window* p_window = bj_malloc(sizeof(bj_window));
    bj_memcpy(p_window, &window, sizeof(bj_window));

    XSaveContext(
        p_x11->display,
        window.handle,
        p_x11->window_context,
        (XPointer) p_window
    );

    /* ///---------------------------------------------- */
    /* int quit = 0; */
    /* while(!quit) { */
    /*     XEvent event = {0}; */
    /*     XNextEvent(p_x11->display, &event); */

    /*     switch(event.type) { */
    /*         case KeyRelease: */
    /*             bj_info("Some key is released"); */
    /*             break; */
    /*         case KeyPress: */
    /*             bj_info("Some key is pressed"); */
    /*             break; */
    /*         case ClientMessage: */
    /*             if((Atom)event.xclient.data.l[0] == p_x11->wm_delete_window) { */
    /*                 quit = true; */
    /*             } */
    /*             break; */
    /*         default: */
    /*             break; */

    /*     } */
    /* } */
    /* ///---------------------------------------------- */

    return p_window;
}

static void x11_delete_window(
    bj_window_backend* p_backend,
    bj_window* p_window
) {
    x11_backend* p_x11 = (x11_backend*)p_backend;
    XDeleteContext(p_x11->display, p_window->handle, p_x11->window_context);
    XUnmapWindow(p_x11->display, p_window->handle);
    XDestroyWindow(p_x11->display, p_window->handle);
    XFlush(p_x11->display);
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


static void x11_poll_events(
    bj_window_backend* p_backend
) {
    assert(p_backend);
    x11_backend* p_x11 = (x11_backend*)p_backend;

    XPending(p_x11->display);

    while(XQLength(p_x11->display)) {
        XEvent event;
        XNextEvent(p_x11->display, &event);

        // Here switch events that do not need window

        bj_window* p_window = 0;
        const int context_res = XFindContext(
            p_x11->display,
            event.xany.window,
            p_x11->window_context,
            (XPointer*) &p_window
        );

        if (context_res != 0) {
            break;
        }

        switch(event.type) {

            case ClientMessage:

                if (event.xclient.message_type == p_x11->wm_protocols) {
                    

                    if (event.xclient.data.l[0] == p_x11->wm_delete_window)
                    {
                        p_window->must_close = true;
                        /* _glfwInputWindowCloseRequest(window); */
                    }
                }
                                
                break;

            default:
                break;
        }
    }
    XFlush(p_x11->display);

}

static bool x11_must_close(
    bj_window_backend* p_backend,
    bj_window* p_window
) {
    return p_window->must_close;
}

static bj_window_backend* x11_init_backend(
    bj_error** p_error
) {
    Display* display = XOpenDisplay(0);
    if(display == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE | X11_CANNOT_OPEN_DISPLAY, "cannot open X11 display");
        return 0;
    }

    bj_debug("X11 display connection: %d", XConnectionNumber(display));

    x11_backend* p_x11 = bj_malloc(sizeof(x11_backend));

    p_x11->fns.dispose = x11_dispose_backend;
    p_x11->fns.create_window = x11_create_window;
    p_x11->fns.delete_window = x11_delete_window;
    p_x11->fns.poll_events   = x11_poll_events;
    p_x11->fns.must_close    = x11_must_close;

    p_x11->display = display;
    p_x11->screen = DefaultScreen(display);
    p_x11->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    p_x11->wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
    p_x11->window_context = XUniqueContext();
    return (bj_window_backend*)p_x11;
}


bj_window_backend_create_info x11_backend_create_info = {
    .name = "x11",
    .create = x11_init_backend,
};

