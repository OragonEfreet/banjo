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
#include <string.h>

#define X11_CANNOT_OPEN_DISPLAY 0x00010000

// sort /(\*\s\zs[^)]\+)/
typedef int                 (* pfn_XCloseDisplay)(Display*);
typedef int                 (* pfn_XDefaultScreen)(Display*);
typedef int                 (* pfn_XFlush)(Display*);
typedef int                 (* pfn_XPending)(Display*);
typedef int                 (* pfn_XQLength)(Display*);
typedef void                (* pfn_XStoreName)(Display*, Window, char*);
typedef int                 (* pfn_XSync)(Display*,Bool);
typedef KeySym*             (* pfn_XGetKeyboardMapping)(Display*,KeyCode,int,int*);
typedef int                 (* pfn_XDestroyWindow)(Display*,Window);
typedef int                 (* pfn_XMapWindow)(Display*,Window);
typedef int                 (* pfn_XUnmapWindow)(Display*,Window);
typedef Status              (* pfn_XSetWMProtocols)(Display*,Window,Atom*,int);
typedef Window              (* pfn_XCreateWindow)(Display*,Window,int,int,unsigned int,unsigned int,unsigned int,int,unsigned int,Visual*,unsigned long,XSetWindowAttributes*);
typedef int                 (* pfn_XNextEvent)(Display*,XEvent*);
typedef int                 (* pfn_XPeekEvent)(Display*,XEvent*);
typedef int                 (* pfn_XDeleteContext)(Display*,XID,XContext);
typedef int                 (* pfn_XFindContext)(Display*,XID,XContext,XPointer*);
typedef int                 (* pfn_XSaveContext)(Display*,XID,XContext,const char*);
typedef Atom                (* pfn_XInternAtom)(Display*,const char*,Bool);
typedef unsigned long       (* pfn_XBlackPixel)(Display*,int);
typedef int                 (* pfn_XDefaultDepth)(Display*,int);
typedef Visual*             (* pfn_XDefaultVisual)(Display*,int);
typedef int                 (* pfn_XEventsQueued)(Display*,int);
typedef int                 (* pfn_XDisplayKeycodes)(Display*,int*,int*);
typedef Display*            (* pfn_XOpenDisplay)(const char*);
typedef XrmQuark            (* pfn_XrmUniqueQuark)(void);
typedef int                 (* pfn_XFree)(void*);
typedef char* (*pfn_XKeysymToString)(KeySym);

#define N_KEYCODES 256

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

    bj_key*           keymap;

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
    uint16_t height,
    uint8_t  flags
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
            .flags      = flags,
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


static int get_key(
    x11_backend* p_x11,
    int          keycode
) {
    if (keycode < 0 || keycode > 255)
        return BJ_KEY_UNKNOWN;

    return p_x11->keymap[keycode];
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
                get_key(p_x11, event->xkey.keycode),
                event->xkey.keycode
            );
            return;

        case KeyPress:

            bj_window_input_key(
                (bj_window*)p_window,
                BJ_PRESS,
                get_key(p_x11, event->xkey.keycode),
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

static int translate_keysyms(const KeySym* keysyms, int width) {
    if (width > 1)
    {
        switch (keysyms[1])
        {
            case XK_KP_0:           return BJ_KEY_NUMPAD0;
            case XK_KP_1:           return BJ_KEY_NUMPAD1;
            case XK_KP_2:           return BJ_KEY_NUMPAD2;
            case XK_KP_3:           return BJ_KEY_NUMPAD3;
            case XK_KP_4:           return BJ_KEY_NUMPAD4;
            case XK_KP_5:           return BJ_KEY_NUMPAD5;
            case XK_KP_6:           return BJ_KEY_NUMPAD6;
            case XK_KP_7:           return BJ_KEY_NUMPAD7;
            case XK_KP_8:           return BJ_KEY_NUMPAD8;
            case XK_KP_9:           return BJ_KEY_NUMPAD9;
            case XK_KP_Separator:
            case XK_KP_Decimal:     return BJ_KEY_DECIMAL;
            default:                break;
        }
    }

    switch (keysyms[0])
    {
        case XK_Escape:         return BJ_KEY_ESCAPE;
        case XK_Tab:            return BJ_KEY_TAB;
        case XK_Shift_L:        return BJ_KEY_LSHIFT;
        case XK_Shift_R:        return BJ_KEY_RSHIFT;
        case XK_Control_L:      return BJ_KEY_LCONTROL;
        case XK_Control_R:      return BJ_KEY_RCONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return BJ_KEY_LMENU;
        case XK_Mode_switch: // Mapped to Alt_R on many keyboards
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Meta_R:
        case XK_Alt_R:          return BJ_KEY_RMENU;
        case XK_Super_L:        return BJ_KEY_LWIN;
        case XK_Super_R:        return BJ_KEY_RWIN;
        case XK_Menu:           return BJ_KEY_MENU;
        case XK_Num_Lock:       return BJ_KEY_NUMLOCK;
        case XK_Caps_Lock:      return BJ_KEY_CAPITAL;
        case XK_Print:          return BJ_KEY_SNAPSHOT;
        case XK_Scroll_Lock:    return BJ_KEY_SCROLL;
        case XK_Pause:          return BJ_KEY_PAUSE;
        case XK_Delete:         return BJ_KEY_DELETE;
        case XK_BackSpace:      return BJ_KEY_BACKSPACE;
        case XK_Return:         return BJ_KEY_RETURN;
        case XK_Home:           return BJ_KEY_HOME;
        case XK_End:            return BJ_KEY_END;
        case XK_Page_Up:        return BJ_KEY_PRIOR;
        case XK_Page_Down:      return BJ_KEY_NEXT;
        case XK_Insert:         return BJ_KEY_INSERT;
        case XK_Left:           return BJ_KEY_LEFT;
        case XK_Right:          return BJ_KEY_RIGHT;
        case XK_Down:           return BJ_KEY_DOWN;
        case XK_Up:             return BJ_KEY_UP;
        case XK_F1:             return BJ_KEY_F1;
        case XK_F2:             return BJ_KEY_F2;
        case XK_F3:             return BJ_KEY_F3;
        case XK_F4:             return BJ_KEY_F4;
        case XK_F5:             return BJ_KEY_F5;
        case XK_F6:             return BJ_KEY_F6;
        case XK_F7:             return BJ_KEY_F7;
        case XK_F8:             return BJ_KEY_F8;
        case XK_F9:             return BJ_KEY_F9;
        case XK_F10:            return BJ_KEY_F10;
        case XK_F11:            return BJ_KEY_F11;
        case XK_F12:            return BJ_KEY_F12;
        case XK_F13:            return BJ_KEY_F13;
        case XK_F14:            return BJ_KEY_F14;
        case XK_F15:            return BJ_KEY_F15;
        case XK_F16:            return BJ_KEY_F16;
        case XK_F17:            return BJ_KEY_F17;
        case XK_F18:            return BJ_KEY_F18;
        case XK_F19:            return BJ_KEY_F19;
        case XK_F20:            return BJ_KEY_F20;
        case XK_F21:            return BJ_KEY_F21;
        case XK_F22:            return BJ_KEY_F22;
        case XK_F23:            return BJ_KEY_F23;
        case XK_F24:            return BJ_KEY_F24;

        // Numeric keypad
        case XK_KP_Divide:      return BJ_KEY_DIVIDE;
        case XK_KP_Multiply:    return BJ_KEY_MULTIPLY;
        case XK_KP_Subtract:    return BJ_KEY_SUBTRACT;
        case XK_KP_Add:         return BJ_KEY_ADD;

        // These should have been detected in secondary keysym test above!
        case XK_KP_Insert:      return BJ_KEY_NUMPAD0;
        case XK_KP_End:         return BJ_KEY_NUMPAD1;
        case XK_KP_Down:        return BJ_KEY_NUMPAD2;
        case XK_KP_Page_Down:   return BJ_KEY_NUMPAD3;
        case XK_KP_Left:        return BJ_KEY_NUMPAD4;
        case XK_KP_Right:       return BJ_KEY_NUMPAD6;
        case XK_KP_Home:        return BJ_KEY_NUMPAD7;
        case XK_KP_Up:          return BJ_KEY_NUMPAD8;
        case XK_KP_Page_Up:     return BJ_KEY_NUMPAD9;
        case XK_KP_Delete:      return BJ_KEY_DECIMAL;

        // Last resort: Check for printable keys (should not happen if the XKB
        // extension is available). This will give a layout dependent mapping
        // (which is wrong, and we may miss some keys, especially on non-US
        // keyboards), but it's better than nothing...
        case XK_a:              return BJ_KEY_A;
        case XK_b:              return BJ_KEY_B;
        case XK_c:              return BJ_KEY_C;
        case XK_d:              return BJ_KEY_D;
        case XK_e:              return BJ_KEY_E;
        case XK_f:              return BJ_KEY_F;
        case XK_g:              return BJ_KEY_G;
        case XK_h:              return BJ_KEY_H;
        case XK_i:              return BJ_KEY_I;
        case XK_j:              return BJ_KEY_J;
        case XK_k:              return BJ_KEY_K;
        case XK_l:              return BJ_KEY_L;
        case XK_m:              return BJ_KEY_M;
        case XK_n:              return BJ_KEY_N;
        case XK_o:              return BJ_KEY_O;
        case XK_p:              return BJ_KEY_P;
        case XK_q:              return BJ_KEY_Q;
        case XK_r:              return BJ_KEY_R;
        case XK_s:              return BJ_KEY_S;
        case XK_t:              return BJ_KEY_T;
        case XK_u:              return BJ_KEY_U;
        case XK_v:              return BJ_KEY_V;
        case XK_w:              return BJ_KEY_W;
        case XK_x:              return BJ_KEY_X;
        case XK_y:              return BJ_KEY_Y;
        case XK_z:              return BJ_KEY_Z;
        case XK_1:              return BJ_KEY_1;
        case XK_2:              return BJ_KEY_2;
        case XK_3:              return BJ_KEY_3;
        case XK_4:              return BJ_KEY_4;
        case XK_5:              return BJ_KEY_5;
        case XK_6:              return BJ_KEY_6;
        case XK_7:              return BJ_KEY_7;
        case XK_8:              return BJ_KEY_8;
        case XK_9:              return BJ_KEY_9;
        case XK_0:              return BJ_KEY_0;
        case XK_space:          return BJ_KEY_SPACE;
        case XK_minus:          return BJ_KEY_MINUS;
        case XK_equal:          return BJ_KEY_OEM_PLUS;
        case XK_bracketleft:    return BJ_KEY_LEFT_BRACKET;
        case XK_bracketright:   return BJ_KEY_RIGHT_BRACKET;
        case XK_backslash:      return BJ_KEY_BACKSLASH;
        case XK_semicolon:      return BJ_KEY_SEMICOLON;
        case XK_apostrophe:     return BJ_KEY_APOSTROPHE;
        case XK_grave:          return BJ_KEY_GRAVE_ACCENT;
        case XK_comma:          return BJ_KEY_COMMA;
        case XK_period:         return BJ_KEY_PERIOD;
        case XK_slash:          return BJ_KEY_SLASH;
        default:                break;
    }

    // No matching translation was found
    return BJ_KEY_UNKNOWN;

}

static void x11_init_keycodes(
    x11_backend* p_x11
) {
    int min_keycode = 0;
    int max_keycode = 0;

    pfn_XDisplayKeycodes    x11_XDisplayKeycodes    = (pfn_XDisplayKeycodes)x11_get_symbol(p_x11, "XDisplayKeycodes");
    pfn_XFree               x11_XFree               = (pfn_XFree)x11_get_symbol(p_x11, "XFree");
    pfn_XGetKeyboardMapping x11_XGetKeyboardMapping = (pfn_XGetKeyboardMapping)x11_get_symbol(p_x11, "XGetKeyboardMapping");


    x11_XDisplayKeycodes(p_x11->display, &min_keycode, &max_keycode);

    int keysym_per_keycode;
    KeySym* keysyms = x11_XGetKeyboardMapping(
        p_x11->display,
        min_keycode,
        max_keycode - min_keycode + 1,
        &keysym_per_keycode
    );

    p_x11->keymap = bj_malloc(sizeof(bj_key) * max_keycode);
    bj_memset(p_x11->keymap, 0, sizeof(bj_key) * max_keycode);

    for (int keycode = min_keycode;  keycode <= max_keycode;  ++keycode) {
        if (p_x11->keymap[keycode] == BJ_KEY_UNKNOWN) {
            const size_t base = (keycode - min_keycode) * keysym_per_keycode;
            p_x11->keymap[keycode] = translate_keysyms(&keysyms[base], keysym_per_keycode);
        }
    }

    x11_XFree(keysyms);
}


static void x11_dispose_backend(
    bj_system_backend* p_backend,
    bj_error** p_error
) {
    (void)p_error;
    x11_backend* p_x11 = (x11_backend*)p_backend;
    pfn_XCloseDisplay XCloseDisplay = (pfn_XCloseDisplay)x11_get_symbol(p_x11, "XCloseDisplay");
    XCloseDisplay(p_x11->display);
    bj_free(p_x11->keymap);
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

    p_x11->XCreateWindow       = (pfn_XCreateWindow)x11_get_symbol(p_x11, "XCreateWindow");
    p_x11->XDeleteContext      = (pfn_XDeleteContext)x11_get_symbol(p_x11, "XDeleteContext");
    p_x11->XDestroyWindow      = (pfn_XDestroyWindow)x11_get_symbol(p_x11, "XDestroyWindow");
    p_x11->XEventsQueued       = (pfn_XEventsQueued)x11_get_symbol(p_x11, "XEventsQueued");
    p_x11->XFindContext        = (pfn_XFindContext)x11_get_symbol(p_x11, "XFindContext");
    p_x11->XFlush              = (pfn_XFlush)x11_get_symbol(p_x11, "XFlush");
    p_x11->XMapWindow          = (pfn_XMapWindow)x11_get_symbol(p_x11, "XMapWindow");
    p_x11->XNextEvent          = (pfn_XNextEvent)x11_get_symbol(p_x11, "XNextEvent");
    p_x11->XPeekEvent          = (pfn_XPeekEvent)x11_get_symbol(p_x11, "XPeekEvent");
    p_x11->XPending            = (pfn_XPending)x11_get_symbol(p_x11, "XPending");
    p_x11->XQLength            = (pfn_XQLength)x11_get_symbol(p_x11, "XQLength");
    p_x11->XSaveContext        = (pfn_XSaveContext)x11_get_symbol(p_x11, "XSaveContext");
    p_x11->XSetWMProtocols     = (pfn_XSetWMProtocols)x11_get_symbol(p_x11, "XSetWMProtocols");
    p_x11->XStoreName          = (pfn_XStoreName)x11_get_symbol(p_x11, "XStoreName");
    p_x11->XSync               = (pfn_XSync)x11_get_symbol(p_x11, "XSync");
    p_x11->XUnmapWindow        = (pfn_XUnmapWindow)x11_get_symbol(p_x11, "XUnmapWindow");
    
    pfn_XBlackPixel         x11_XBlackPixel       = (pfn_XBlackPixel)x11_get_symbol(p_x11, "XBlackPixel");
    pfn_XDefaultScreen      x11_XDefaultScreen    = (pfn_XDefaultScreen)x11_get_symbol(p_x11, "XDefaultScreen");
    pfn_XDefaultDepth       x11_XDefaultDepth     = (pfn_XDefaultDepth)x11_get_symbol(p_x11, "XDefaultDepth");
    pfn_XDefaultVisual      x11_XDefaultVisual    = (pfn_XDefaultVisual)x11_get_symbol(p_x11, "XDefaultVisual");
    pfn_XInternAtom         x11_XInternAtom       = (pfn_XInternAtom)x11_get_symbol(p_x11, "XInternAtom");
    pfn_XOpenDisplay        x11_XOpenDisplay      = (pfn_XOpenDisplay)x11_get_symbol(p_x11, "XOpenDisplay");
    pfn_XrmUniqueQuark      x11_XrmUniqueQuark    = (pfn_XrmUniqueQuark)x11_get_symbol(p_x11, "XrmUniqueQuark");

    Display* display = x11_XOpenDisplay(0);
    if(display == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE | X11_CANNOT_OPEN_DISPLAY, "cannot open X11 display");
        return 0;
    }

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
