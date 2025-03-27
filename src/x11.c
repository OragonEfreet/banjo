#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/window.h>

#include "config.h"

#if BJ_HAS_FEATURE(X11)

#include "system_t.h"
#include "window_t.h"

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include <assert.h>

#define X11_CANNOT_OPEN_DISPLAY 0x00010000

typedef Atom                (* pfn_XInternAtom)(Display*,const char*,Bool);
typedef Display*            (* pfn_XOpenDisplay)(const char*);
typedef KeySym*             (* pfn_XGetKeyboardMapping)(Display*,KeyCode,int,int*);
typedef Status              (* pfn_XSetWMProtocols)(Display*,Window,Atom*,int);
typedef Visual*             (* pfn_XDefaultVisual)(Display*,int);
typedef Window              (* pfn_XCreateWindow)(Display*,Window,int,int,unsigned int,unsigned int,unsigned int,int,unsigned int,Visual*,unsigned long,XSetWindowAttributes*);
typedef XrmQuark            (* pfn_XrmUniqueQuark)(void);
typedef int                 (* pfn_XCloseDisplay)(Display*);
typedef int                 (* pfn_XDefaultDepth)(Display*,int);
typedef int                 (* pfn_XDefaultScreen)(Display*);
typedef int                 (* pfn_XDeleteContext)(Display*,XID,XContext);
typedef int                 (* pfn_XDestroyWindow)(Display*,Window);
typedef int                 (* pfn_XDisplayKeycodes)(Display*,int*,int*);
typedef int                 (* pfn_XEventsQueued)(Display*,int);
typedef int                 (* pfn_XFindContext)(Display*,XID,XContext,XPointer*);
typedef int                 (* pfn_XFlush)(Display*);
typedef int                 (* pfn_XFree)(void*);
typedef int                 (* pfn_XMapWindow)(Display*,Window);
typedef int                 (* pfn_XNextEvent)(Display*,XEvent*);
typedef int                 (* pfn_XPeekEvent)(Display*,XEvent*);
typedef int                 (* pfn_XPending)(Display*);
typedef int                 (* pfn_XQLength)(Display*);
typedef int                 (* pfn_XSaveContext)(Display*,XID,XContext,const char*);
typedef int                 (* pfn_XSync)(Display*,Bool);
typedef int                 (* pfn_XUnmapWindow)(Display*,Window);
typedef unsigned long       (* pfn_XBlackPixel)(Display*,int);
typedef void                (* pfn_XStoreName)(Display*, Window, char*);


typedef struct {
    bj_system_backend fns;
    void*             p_handle;
    Display*          display;
    int               default_screen;
    int               default_depth;
    Visual*           default_visual;
    unsigned long     black_pixel;
    Atom              wm_protocols;
    Atom              wm_delete_window;
    XContext          window_context;

    pfn_XCreateWindow       XCreateWindow;
    pfn_XDeleteContext      XDeleteContext;
    pfn_XDestroyWindow      XDestroyWindow;
    pfn_XEventsQueued       XEventsQueued;
    pfn_XFindContext        XFindContext;
    pfn_XFlush              XFlush;
    pfn_XMapWindow          XMapWindow;
    pfn_XNextEvent          XNextEvent;
    pfn_XPeekEvent          XPeekEvent;
    pfn_XPending            XPending;
    pfn_XQLength            XQLength;
    pfn_XSaveContext        XSaveContext;
    pfn_XSetWMProtocols     XSetWMProtocols;
    pfn_XStoreName          XStoreName;
    pfn_XSync               XSync;
    pfn_XUnmapWindow        XUnmapWindow;

} x11_backend;

typedef struct {
    struct bj_window_t common;
    Window             handle;
} x11_window;

static void* x11_get_symbol(
    x11_backend*  p_x11,
    const char* p_name
) {
    return bj_get_symbol(p_x11->p_handle, p_name);
}


static bj_window* x11_create_window(
    bj_system_backend* p_backend,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
) {
    x11_backend* p_x11 = (x11_backend*)p_backend;
    Window root_window = RootWindow(p_x11->display, p_x11->default_screen);

    XSetWindowAttributes attributes = {
        .background_pixel = p_x11->black_pixel,
        .border_pixel     = p_x11->black_pixel,
        .event_mask       =   ButtonReleaseMask | ButtonPressMask
                            | KeyReleaseMask    | KeyPressMask
                            | PointerMotionMask
                            | EnterWindowMask    | LeaveWindowMask
    };

    x11_window window = { 
        .common = {
            .must_close = false,
        },
        .handle = p_x11->XCreateWindow(
            p_x11->display, root_window,
            x, y,
            width, height, 1,
            p_x11->default_depth,
            InputOutput,
            p_x11->default_visual,
            CWBackPixel | CWBorderPixel | CWEventMask, &attributes
        ),
    };

    p_x11->XStoreName(p_x11->display, window.handle, (char*)p_title);

    p_x11->XSetWMProtocols(
        p_x11->display, window.handle,
        &p_x11->wm_delete_window, 1
    );

    p_x11->XMapWindow(p_x11->display, window.handle);
    p_x11->XSync(p_x11->display, 0);

    
    x11_window* p_window = bj_malloc(sizeof(x11_window));
    bj_memcpy(p_window, &window, sizeof(x11_window));

    p_x11->XSaveContext(
        p_x11->display,
        window.handle,
        p_x11->window_context,
        (XPointer) p_window
    );

    return (bj_window*)p_window;
}

static void x11_delete_window(
    bj_system_backend* p_backend,
    bj_window* p_abstract_window
) {
    x11_window* p_window = (x11_window*)p_abstract_window;
    x11_backend* p_x11 = (x11_backend*)p_backend;
    p_x11->XDeleteContext(p_x11->display, p_window->handle, p_x11->window_context);
    p_x11->XUnmapWindow(p_x11->display, p_window->handle);
    p_x11->XDestroyWindow(p_x11->display, p_window->handle);
    p_x11->XFlush(p_x11->display);
    bj_free(p_window);
}

static void x11_dispatch_event(
    x11_backend*  p_x11,
    const XEvent* event
) {
    // Here switch events that do not need window

    x11_window* p_window = 0;
    const int context_res = p_x11->XFindContext(
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
                
                if ((Atom)event->xclient.data.l[0] == p_x11->wm_delete_window) {
                    bj_window_set_should_close(&p_window->common);
                }
            }
            return;

        case KeyRelease:
            if (p_x11->XEventsQueued(p_x11->display, QueuedAfterReading))
            {
                XEvent next;
                p_x11->XPeekEvent(p_x11->display, &next);

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
    bj_system_backend* p_backend
) {
    assert(p_backend);
    x11_backend* p_x11 = (x11_backend*)p_backend;

    p_x11->XPending(p_x11->display);

    while(p_x11->XQLength(p_x11->display)) {
        XEvent event;
        p_x11->XNextEvent(p_x11->display, &event);
        x11_dispatch_event(p_x11, &event);
    }
    p_x11->XFlush(p_x11->display);
}

static void x11_init_keycodes(
    x11_backend* p_x11
) {
    pfn_XDisplayKeycodes    x11_XDisplayKeycodes    = x11_get_symbol(p_x11, "XDisplayKeycodes");
    pfn_XFree               x11_XFree               = x11_get_symbol(p_x11, "XFree");
    pfn_XGetKeyboardMapping x11_XGetKeyboardMapping = x11_get_symbol(p_x11, "XGetKeyboardMapping");

    int min_keycodes = 0;
    int max_keycodes = 0;

    x11_XDisplayKeycodes(p_x11->display, &min_keycodes, &max_keycodes);

    int keysym_per_keycode;
    KeySym* keysyms = x11_XGetKeyboardMapping(
        p_x11->display,
        min_keycodes,
        max_keycodes - min_keycodes + 1,
        &keysym_per_keycode
    );

    x11_XFree(keysyms);
}

static void x11_dispose_backend(
    bj_system_backend* p_backend,
    bj_error** p_error
) {
    (void)p_error;
    x11_backend* p_x11 = (x11_backend*)p_backend;
    pfn_XCloseDisplay XCloseDisplay = x11_get_symbol(p_x11, "XCloseDisplay");
    XCloseDisplay(p_x11->display);
    bj_free(p_backend);
}

static bj_system_backend* x11_init_backend(
    bj_error** p_error
) {

    void* p_handle = bj_load_library("libX11.so.6");
    if(p_handle == 0) {
        return 0;
    }

    x11_backend* p_x11 = bj_malloc(sizeof(x11_backend));
    p_x11->p_handle            = p_handle;

    p_x11->XCreateWindow       = x11_get_symbol(p_x11, "XCreateWindow");
    p_x11->XDeleteContext      = x11_get_symbol(p_x11, "XDeleteContext");
    p_x11->XDestroyWindow      = x11_get_symbol(p_x11, "XDestroyWindow");
    p_x11->XEventsQueued       = x11_get_symbol(p_x11, "XEventsQueued");
    p_x11->XFindContext        = x11_get_symbol(p_x11, "XFindContext");
    p_x11->XFlush              = x11_get_symbol(p_x11, "XFlush");
    p_x11->XMapWindow          = x11_get_symbol(p_x11, "XMapWindow");
    p_x11->XNextEvent          = x11_get_symbol(p_x11, "XNextEvent");
    p_x11->XPeekEvent          = x11_get_symbol(p_x11, "XPeekEvent");
    p_x11->XPending            = x11_get_symbol(p_x11, "XPending");
    p_x11->XQLength            = x11_get_symbol(p_x11, "XQLength");
    p_x11->XSaveContext        = x11_get_symbol(p_x11, "XSaveContext");
    p_x11->XSetWMProtocols     = x11_get_symbol(p_x11, "XSetWMProtocols");
    p_x11->XStoreName          = x11_get_symbol(p_x11, "XStoreName");
    p_x11->XSync               = x11_get_symbol(p_x11, "XSync");
    p_x11->XUnmapWindow        = x11_get_symbol(p_x11, "XUnmapWindow");
    
    pfn_XBlackPixel         x11_XBlackPixel    = x11_get_symbol(p_x11, "XBlackPixel");
    pfn_XDefaultScreen      x11_XDefaultScreen = x11_get_symbol(p_x11, "XDefaultScreen");
    pfn_XDefaultDepth       x11_XDefaultDepth  = x11_get_symbol(p_x11, "XDefaultDepth");
    pfn_XDefaultVisual      x11_XDefaultVisual = x11_get_symbol(p_x11, "XDefaultVisual");
    pfn_XInternAtom         x11_XInternAtom    = x11_get_symbol(p_x11, "XInternAtom");
    pfn_XOpenDisplay        x11_XOpenDisplay   = x11_get_symbol(p_x11, "XOpenDisplay");
    pfn_XrmUniqueQuark      x11_XrmUniqueQuark = x11_get_symbol(p_x11, "XrmUniqueQuark");

    Display* display = x11_XOpenDisplay(0);
    if(display == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE | X11_CANNOT_OPEN_DISPLAY, "cannot open X11 display");
        return 0;
    }

    /* bj_debug("X11 display connection: %d", XConnectionNumber(display)); */
    bj_debug("X11 connected");

    p_x11->display          = display;
    p_x11->default_screen   = x11_XDefaultScreen(display);
    p_x11->default_depth    = x11_XDefaultDepth(display, p_x11->default_screen);
    p_x11->default_visual   = x11_XDefaultVisual(display, p_x11->default_screen),
    p_x11->black_pixel      = x11_XBlackPixel(display, p_x11->default_screen),

    p_x11->wm_delete_window = x11_XInternAtom(display, "WM_DELETE_WINDOW", False);
    p_x11->wm_protocols     = x11_XInternAtom(display, "WM_PROTOCOLS", False);
    p_x11->window_context   = x11_XrmUniqueQuark();

    x11_init_keycodes(p_x11);

    p_x11->fns.dispose       = x11_dispose_backend;
    p_x11->fns.create_window = x11_create_window;
    p_x11->fns.delete_window = x11_delete_window;
    p_x11->fns.poll_events   = x11_poll_events;

    return (bj_system_backend*)p_x11;
}


bj_system_backend_create_info x11_backend_create_info = {
    .name = "x11",
    .create = x11_init_backend,
};

#endif
