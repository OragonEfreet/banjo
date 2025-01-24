#include <banjo/error.h>
#include <banjo/memory.h>
#include <banjo/window.h>
#include <banjo/log.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include "window_backend.h"
#include "window_t.h"

#define X11_CANNOT_OPEN_DISPLAY 0x00010000

typedef struct {
    bj_window_backend fns;
    Display*          display;
    int               screen;
    Atom              wm_protocols;
    Atom              wm_delete_window;
    XContext          window_context;
} x11_backend;

typedef struct {
    struct bj_window_t common;
    Window             handle;
} x11_window;

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
        .event_mask       =   ButtonReleaseMask | ButtonPressMask
                            | KeyReleaseMask    | KeyPressMask
                            | PointerMotionMask
                            | EnterWindowMask    | LeaveWindowMask
    };

    x11_window window = { 
        .common = {
            .must_close = false,
        },
        .handle = XCreateWindow(
            p_x11->display, root_window,
            x, y,
            width, height, 1,
            DefaultDepth(p_x11->display, p_x11->screen),
            InputOutput,
            DefaultVisual(p_x11->display, p_x11->screen),
            CWBackPixel | CWBorderPixel | CWEventMask, &attributes
        ),
    };

    XSetWMProtocols(
        p_x11->display, window.handle,
        &p_x11->wm_delete_window, 1
    );


    XMapWindow(p_x11->display, window.handle);
    XSync(p_x11->display, 0);

    
    x11_window* p_window = bj_malloc(sizeof(x11_window));
    bj_memcpy(p_window, &window, sizeof(x11_window));

    XSaveContext(
        p_x11->display,
        window.handle,
        p_x11->window_context,
        (XPointer) p_window
    );

    return (bj_window*)p_window;
}

static void x11_delete_window(
    bj_window_backend* p_backend,
    bj_window* p_abstract_window
) {
    x11_window* p_window = (x11_window*)p_abstract_window;
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

static void x11_dispatch_event(
    x11_backend*  p_x11,
    const XEvent* event
) {
    // Here switch events that do not need window

    x11_window* p_window = 0;
    const int context_res = XFindContext(
        p_x11->display,
        event->xany.window,
        p_x11->window_context,
        (XPointer*) &p_window
    );

    if (context_res != 0) {
        return;
    }

    switch(event->type) {

        case EnterNotify:
        case LeaveNotify:
            bj_window_input_enter(
                (bj_window*)p_window,
                event->type == EnterNotify,
                event->xcrossing.x,
                event->xcrossing.y
            );
            break;

        case ButtonRelease:
        case ButtonPress:
            bj_window_input_button(
                (bj_window*)p_window,
                event->xbutton.button,
                event->type == ButtonPress ? BJ_PRESS : BJ_RELEASE,
                event->xbutton.x,
                event->xbutton.y
            );

            break;

        case MotionNotify:
            bj_window_input_cursor(
                (bj_window*)p_window,
                event->xmotion.x,
                event->xmotion.y
            );

            break;

        case ClientMessage:

            if (event->xclient.message_type == p_x11->wm_protocols) {
                
                if (event->xclient.data.l[0] == p_x11->wm_delete_window) {
                    bj_window_set_should_close(&p_window->common);
                }
            }
            return;

        case KeyRelease:
            if (XEventsQueued(p_x11->display, QueuedAfterReading))
            {
                XEvent next;
                XPeekEvent(p_x11->display, &next);

                if (next.type == KeyPress &&
                    next.xkey.window == event->xkey.window &&
                    next.xkey.keycode == event->xkey.keycode)
                {
                    if ((next.xkey.time - event->xkey.time) < 20)
                    {
                        return;
                    }
                }
            }
            bj_window_input_key(
                (bj_window*)p_window,
                BJ_RELEASE,
                event->xkey.keycode
            );
            return;

        case KeyPress:
            bj_window_input_key(
                (bj_window*)p_window,
                BJ_PRESS,
                event->xkey.keycode
            );
            return;
    }

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
        x11_dispatch_event(p_x11, &event);
    }
    XFlush(p_x11->display);

}

static void x11_init_keycodes(
    x11_backend* p_x11
) {
    int min_keycodes = 0;
    int max_keycodes = 0;

    XDisplayKeycodes(p_x11->display, &min_keycodes, &max_keycodes);

    int keysym_per_keycode;
    KeySym* keysyms = XGetKeyboardMapping(
        p_x11->display,
        min_keycodes,
        max_keycodes - min_keycodes + 1,
        &keysym_per_keycode
    );

    XFree(keysyms);
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

    p_x11->fns.dispose       = x11_dispose_backend;
    p_x11->fns.create_window = x11_create_window;
    p_x11->fns.delete_window = x11_delete_window;
    p_x11->fns.poll_events   = x11_poll_events;

    p_x11->display = display;
    p_x11->screen = DefaultScreen(display);
    p_x11->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    p_x11->wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
    p_x11->window_context = XUniqueContext();

    x11_init_keycodes(p_x11);

    return (bj_window_backend*)p_x11;
}

bj_window_backend_create_info x11_backend_create_info = {
    .name = "x11",
    .create = x11_init_backend,
};

