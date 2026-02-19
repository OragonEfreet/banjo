#ifdef BJ_CONFIG_X11_BACKEND

#include <banjo/assert.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#include <bitmap.h>
#include <check.h>
#include <renderer.h>
#include <video_layer.h>
#include <window.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include <string.h> /* memcpy for C99-safe void*-to-function-pointer loading */

/* C99-safe dlsym wrapper: avoids void*-to-function-pointer cast (ISO C forbids it). */
#define LOAD_SYM(dest, handle, name) do { \
    void* sym_ = bj_library_symbol((handle), (name), 0); \
    memcpy(&(dest), &sym_, sizeof(dest)); \
} while(0)

typedef XSizeHints*         (* pfn_XAllocSizeHints)(void);
typedef unsigned long       (* pfn_XBlackPixel)(Display*,int);
typedef int                 (* pfn_XCloseDisplay)(Display*);
typedef GC                  (* pfn_XCreateGC)(Display*,Drawable,unsigned long,XGCValues*);
typedef XImage*             (* pfn_XCreateImage)(Display*,Visual*,unsigned int,int,int,char*,unsigned int,unsigned int,int,int);
typedef Window              (* pfn_XCreateWindow)(Display*,Window,int,int,unsigned int,unsigned int,unsigned int,int,unsigned int,Visual*,unsigned long,XSetWindowAttributes*);
typedef int                 (* pfn_XDefaultDepth)(Display*,int);
typedef int                 (* pfn_XDefaultScreen)(Display*);
typedef Visual*             (* pfn_XDefaultVisual)(Display*,int);
typedef int                 (* pfn_XDeleteContext)(Display*,XID,XContext);
typedef int                 (* pfn_XDestroyWindow)(Display*,Window);
typedef int                 (* pfn_XDisplayKeycodes)(Display*,int*,int*);
typedef int                 (* pfn_XEventsQueued)(Display*,int);
typedef int                 (* pfn_XFindContext)(Display*,XID,XContext,XPointer*);
typedef int                 (* pfn_XFlush)(Display*);
typedef int                 (* pfn_XFree)(void*);
typedef int                 (* pfn_XFreeGC)(Display*,GC);
typedef KeySym*             (* pfn_XGetKeyboardMapping)(Display*,KeyCode,int,int*);
typedef Status              (* pfn_XGetWindowAttributes)(Display*,Window,XWindowAttributes*);
typedef Atom                (* pfn_XInternAtom)(Display*,const char*,Bool);
typedef char*               (* pfn_XKeysymToString)(KeySym);
typedef int                 (* pfn_XMapWindow)(Display*,Window);
typedef int                 (* pfn_XNextEvent)(Display*,XEvent*);
typedef Display*            (* pfn_XOpenDisplay)(const char*);
typedef int                 (* pfn_XPeekEvent)(Display*,XEvent*);
typedef int                 (* pfn_XPending)(Display*);
typedef int                 (* pfn_XPutImage)(Display*,Drawable,GC,XImage*,int,int,int,int,unsigned int,unsigned int);
typedef int                 (* pfn_XQLength)(Display*);
typedef int                 (* pfn_XSaveContext)(Display*,XID,XContext,const char*);
typedef void                (* pfn_XSetWMNormalHints)(Display*,Window,XSizeHints*);
typedef Status              (* pfn_XSetWMProtocols)(Display*,Window,Atom*,int);
typedef void                (* pfn_XStoreName)(Display*, Window, char*);
typedef int                 (* pfn_XSync)(Display*,Bool);
typedef int                 (* pfn_XUnmapWindow)(Display*,Window);
typedef XrmQuark            (* pfn_XrmUniqueQuark)(void);

#define N_KEYCODES 256


struct {
    void*             handle;
    Display*          display;
    int               default_screen;
    int               default_depth;
    Visual*           default_visual;
    unsigned long     black_pixel;
    Atom              wm_protocols;
    Atom              wm_delete_window;
    XContext          window_context;

    enum bj_key*           keymap;

    pfn_XAllocSizeHints      XAllocSizeHints;
    pfn_XCreateGC            XCreateGC;
    pfn_XCreateImage         XCreateImage;
    pfn_XCreateWindow        XCreateWindow;
    pfn_XDeleteContext       XDeleteContext;
    pfn_XDestroyWindow       XDestroyWindow;
    pfn_XEventsQueued        XEventsQueued;
    pfn_XFindContext         XFindContext;
    pfn_XFlush               XFlush;
    pfn_XFree                XFree;
    pfn_XFreeGC              XFreeGC;
    pfn_XGetWindowAttributes XGetWindowAttributes;
    pfn_XMapWindow           XMapWindow;
    pfn_XNextEvent           XNextEvent;
    pfn_XPeekEvent           XPeekEvent;
    pfn_XPending             XPending;
    pfn_XPutImage            XPutImage;
    pfn_XQLength             XQLength;
    pfn_XSaveContext         XSaveContext;
    pfn_XSetWMNormalHints    XSetWMNormalHints;
    pfn_XSetWMProtocols      XSetWMProtocols;
    pfn_XStoreName           XStoreName;
    pfn_XSync                XSync;
    pfn_XUnmapWindow         XUnmapWindow;
} x11;

struct bj_renderer_data {
    XImage*                     framebuffer_image;
    void*                       framebuffer_pixels;
    struct bj_bitmap            framebuffer;
};

typedef struct {
    struct bj_window common;
    Window           handle;
} x11_window;

static void x11_wait_for_map_notify(struct bj_video_layer_data* ignore, Window window) {
    (void)ignore;
    double start_time = bj_run_time();

    while ((bj_run_time() - start_time) < 1.0) {
        while (x11.XPending(x11.display)) {
            XEvent event;
            x11.XNextEvent(x11.display, &event);

            if (event.type == MapNotify && event.xmap.window == window) {
                return; // Successfully mapped
            }
        }

        bj_sleep(1); // sleep for 1 ms to avoid busy waiting
    }
}

static struct bj_window* x11_create_window(
    const char*       title,
    uint16_t          x,
    uint16_t          y,
    uint16_t          width,
    uint16_t          height,
    uint8_t           flags,
    struct bj_error** error
) {
    Window root_window = RootWindow(x11.display, x11.default_screen);

    XSetWindowAttributes attributes = {
        .background_pixel = x11.black_pixel,
        .border_pixel     = x11.black_pixel,
        .event_mask       =   ButtonReleaseMask | ButtonPressMask
                            | KeyReleaseMask    | KeyPressMask
                            | PointerMotionMask | StructureNotifyMask
                            | EnterWindowMask   | LeaveWindowMask
    };

    Window handle = x11.XCreateWindow(
        x11.display, root_window,
        x, y,
        width, height, 1,
        x11.default_depth,
        InputOutput,
        x11.default_visual,
        CWBackPixel | CWBorderPixel | CWEventMask, &attributes
    );

    if (handle == 0) {
        bj_set_error(error, BJ_ERROR_VIDEO, "Failed to create X11 window");
        return 0;
    }

    x11_window window = {
        .common = {
            .flags = flags,
        },
        .handle = handle,
    };

    // For now we don't want to make the window resizable
    XSizeHints *size_hints = x11.XAllocSizeHints();
    if (size_hints) {
        size_hints->flags      = PMinSize | PMaxSize;
        size_hints->min_width  = width;
        size_hints->min_height = height;
        size_hints->max_width  = width;
        size_hints->max_height = height;
        x11.XSetWMNormalHints(x11.display, window.handle, size_hints);
        x11.XFree(size_hints);
    }

    x11.XStoreName(x11.display, window.handle, (char*)title);

    x11.XSetWMProtocols(
        x11.display, window.handle,
        &x11.wm_delete_window, 1
    );

    x11.XMapWindow(x11.display, window.handle);
    x11.XSync(x11.display, 0);

    x11_window* window_ptr = bj_malloc(sizeof(x11_window));
    if (window_ptr == 0) {
        x11.XDestroyWindow(x11.display, handle);
        bj_set_error(error, BJ_ERROR_VIDEO, "Failed to allocate window structure");
        return 0;
    }
    bj_memcpy(window_ptr, &window, sizeof(x11_window));

    x11.XSaveContext(
        x11.display,
        window.handle,
        x11.window_context,
        (XPointer) window_ptr
    );

    x11_wait_for_map_notify(0, window.handle);

    return (struct bj_window*)window_ptr;
}

static void x11_delete_window(
    struct bj_window* abstract_window
) {
    x11_window* window = (x11_window*)abstract_window;
    x11.XDeleteContext(x11.display, window->handle, x11.window_context);
    x11.XUnmapWindow(x11.display, window->handle);
    x11.XDestroyWindow(x11.display, window->handle);
    x11.XFlush(x11.display);
    bj_free(window);
}


static enum bj_key get_key(struct bj_video_layer_data* ignore, int keycode) {
    (void)ignore;
    if (keycode < 0 || keycode > 255) {
        return BJ_KEY_UNKNOWN;
    }
    return x11.keymap[keycode];
}

static void x11_dispatch_callback(
    struct bj_video_layer_data*  ignore,
    const XEvent* event
) {
    (void)ignore;
    // Here switch events that do not need window

    x11_window* window = 0;
    const int context_res = x11.XFindContext(
        x11.display,
        event->xany.window,
        x11.window_context,
        (XPointer*) &window
    );

    if (context_res != 0) {
        return;
    }

    switch(event->type) {

        case Expose:
        case EnterNotify:
        case LeaveNotify:
            bj_push_enter_event(
                (struct bj_window*)window,
                event->type == EnterNotify,
                event->xcrossing.x,
                event->xcrossing.y
            );
            break;

        case ButtonRelease:
        case ButtonPress:
            bj_push_button_event(
                (struct bj_window*)window,
                (int)event->xbutton.button,
                event->type == ButtonPress ? BJ_PRESS : BJ_RELEASE,
                event->xbutton.x,
                event->xbutton.y
            );

            break;

        case MotionNotify:
            bj_push_cursor_event(
                (struct bj_window*)window,
                event->xmotion.x,
                event->xmotion.y
            );

            break;

        case ClientMessage:

            if (event->xclient.message_type == x11.wm_protocols) {

                if ((Atom)event->xclient.data.l[0] == x11.wm_delete_window) {
                    bj_set_window_should_close(&window->common);
                }
            }
            return;

        case KeyRelease:
            if (x11.XEventsQueued(x11.display, QueuedAfterReading))
            {
                XEvent next;
                x11.XPeekEvent(x11.display, &next);

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
            bj_push_key_event(
                (struct bj_window*)window,
                BJ_RELEASE,
                get_key(0, (int)event->xkey.keycode),
                (int)event->xkey.keycode
            );
            return;

        case KeyPress:

            bj_push_key_event(
                (struct bj_window*)window,
                BJ_PRESS,
                get_key(0, (int)event->xkey.keycode),
                (int)event->xkey.keycode
            );
            return;
    }

}

static void x11_poll_events(
    void
) {
    x11.XPending(x11.display);

    while(x11.XQLength(x11.display)) {
        XEvent event;
        x11.XNextEvent(x11.display, &event);
        x11_dispatch_callback(0, &event);
    }
    x11.XFlush(x11.display);
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
    struct bj_video_layer_data* ignore
) {
    (void)ignore;
    int min_keycode = 0;
    int max_keycode = 0;

    pfn_XDisplayKeycodes    x11_XDisplayKeycodes;
    pfn_XGetKeyboardMapping x11_XGetKeyboardMapping;
    LOAD_SYM(x11_XDisplayKeycodes, x11.handle, "XDisplayKeycodes");
    LOAD_SYM(x11_XGetKeyboardMapping, x11.handle, "XGetKeyboardMapping");


    x11_XDisplayKeycodes(x11.display, &min_keycode, &max_keycode);

    int keysym_per_keycode;
    KeySym* keysyms = x11_XGetKeyboardMapping(
        x11.display,
        (KeyCode)min_keycode,
        max_keycode - min_keycode + 1,
        &keysym_per_keycode
    );

    x11.keymap = bj_malloc(sizeof(enum bj_key) * (size_t)(max_keycode+1));
    if (x11.keymap == 0) {
        x11.XFree(keysyms);
        return;
    }
    bj_memset(x11.keymap, 0, sizeof(enum bj_key) * (size_t)(max_keycode+1));

    for (int keycode = min_keycode;  keycode <= max_keycode;  ++keycode) {
        if (x11.keymap[keycode] == BJ_KEY_UNKNOWN) {
            const size_t base = (size_t)(keycode - min_keycode) * (size_t)keysym_per_keycode;
            x11.keymap[keycode] = (enum bj_key)translate_keysyms(&keysyms[base], keysym_per_keycode);
        }
    }

    x11.XFree(keysyms);
}

static int x11_get_window_size(
    const struct bj_window* window,
    int* width,
    int* height
) {
    XWindowAttributes attributes;
    x11.XGetWindowAttributes(
        x11.display,
        ((x11_window*)window)->handle,
        &attributes
    );

    if (width) {
        *width = attributes.width;
    }
    if (height) {
        *height = attributes.height;
    }

    return 1;
}

static int bj_visual_to_pixel_mode(const Visual* visual, unsigned int depth) {
    if (!visual) {
        return BJ_PIXEL_MODE_UNKNOWN;
    }

    if (visual->class == PseudoColor || visual->class == StaticColor) {
        if (depth == 8) return BJ_PIXEL_MODE_INDEXED_8;
        if (depth == 4) return BJ_PIXEL_MODE_INDEXED_4;
        if (depth == 1) return BJ_PIXEL_MODE_INDEXED_1;
        return BJ_PIXEL_MODE_UNKNOWN;
    }

    if (visual->class == TrueColor || visual->class == DirectColor) {
        if (depth == 16) {
            if (visual->red_mask == 0x7C00 &&
                visual->green_mask == 0x03E0 &&
                visual->blue_mask == 0x001F) {
                return BJ_PIXEL_MODE_XRGB1555;
            }

            if (visual->red_mask == 0xF800 &&
                visual->green_mask == 0x07E0 &&
                visual->blue_mask == 0x001F) {
                return BJ_PIXEL_MODE_RGB565;
            }
        }

        if (depth == 24 || depth == 32) {
            if (visual->red_mask == 0x00FF0000 &&
                visual->green_mask == 0x0000FF00 &&
                visual->blue_mask == 0x000000FF) {
                return BJ_PIXEL_MODE_XRGB8888;
            }

            if (visual->red_mask == 0x000000FF &&
                visual->green_mask == 0x0000FF00 &&
                visual->blue_mask == 0x00FF0000) {
                return BJ_PIXEL_MODE_BGR24;
            }
        }
    }

    return BJ_PIXEL_MODE_UNKNOWN;
}

static void x11_end_video(
    struct bj_error** error
) {
    (void)error;
    pfn_XCloseDisplay fn_XCloseDisplay;
    LOAD_SYM(fn_XCloseDisplay, x11.handle, "XCloseDisplay");
    fn_XCloseDisplay(x11.display);
    bj_free(x11.keymap);
}

static bj_bool x11_renderer_configure(
    struct bj_renderer* renderer,
    struct bj_window*   window,
    struct bj_error**   error
) {
    XWindowAttributes attributes;

    x11.XGetWindowAttributes(
        x11.display,
        ((x11_window*)window)->handle,
        &attributes
    );

    const enum bj_pixel_mode mode = (enum bj_pixel_mode)bj_visual_to_pixel_mode(
        attributes.visual, (unsigned int)attributes.depth
    );

    if (mode == BJ_PIXEL_MODE_UNKNOWN) {
        bj_set_error(error, BJ_ERROR_VIDEO, "Unsupported X11 visual pixel format");
        return BJ_FALSE;
    }

    // Clean up old XImage if it exists
    if (renderer->data->framebuffer_image) {
        x11.XFree(renderer->data->framebuffer_image);
        renderer->data->framebuffer_image = 0;
    }

    // Reassign the bitmap internals instead of creating a new one
    bj_assign_bitmap(
        &renderer->data->framebuffer,
        0,  // Let it allocate its own buffer
        (size_t)attributes.width,
        (size_t)attributes.height,
        mode,
        0  // Auto-compute stride
    );


    renderer->data->framebuffer_pixels = bj_bitmap_pixels(&renderer->data->framebuffer);
    renderer->data->framebuffer.weak = 1;

    // Note: don't use XDestroyImage to delete this structure, use XFree.
    // Otherwise, XLib will XFree the pixels buffer as well.
    renderer->data->framebuffer_image = x11.XCreateImage(
        x11.display,              // X Display
        attributes.visual,           // Window Visual
        (unsigned int)attributes.depth, // Window Depth
        ZPixmap,                     // Format
        0,                           // Offset
        renderer->data->framebuffer_pixels, // Pixel data
        (unsigned int)attributes.width,  // Width in pixels
        (unsigned int)attributes.height, // Height in pixels
        32,                          // pad
        (int)bj_bitmap_stride(&renderer->data->framebuffer)   // stride
    );
    if (renderer->data->framebuffer_image == 0) {
        bj_set_error(error, BJ_ERROR_VIDEO, "XCreateImage failed");
        return BJ_FALSE;
    }

    return BJ_TRUE;
}

static struct bj_bitmap* x11_renderer_get_framebuffer(
    struct bj_renderer* renderer
) {
    return &renderer->data->framebuffer;
}

static void x11_renderer_present(
    struct bj_renderer* renderer,
    struct bj_window* abstract_window
) {
    Display* display = x11.display;
    x11_window* window = (x11_window*)abstract_window;
    Window window_handle = window->handle;

    int width = 0;
    int height = 0;
    bj_get_window_size(abstract_window, &width, &height);

    GC gc = x11.XCreateGC(display, window_handle, 0, 0);
    x11.XPutImage(
        display, window_handle, gc,
        renderer->data->framebuffer_image,
        0, 0, 0, 0, (unsigned int)width, (unsigned int)height
    );
    x11.XFreeGC(display, gc);
    x11.XSync(display, False);
}

static struct bj_renderer* x11_create_renderer(
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

    renderer->configure       = x11_renderer_configure;
    renderer->get_framebuffer = x11_renderer_get_framebuffer;
    renderer->present         = x11_renderer_present;

    return renderer;
}

static void x11_destroy_renderer(
    struct bj_renderer* renderer
) {
    bj_check(renderer);

    // Clean up the framebuffer bitmap internals
    bj_reset_bitmap(&renderer->data->framebuffer);

    // Clean up XImage if it exists
    if (renderer->data->framebuffer_image) {
        x11.XFree(renderer->data->framebuffer_image);
    }

    bj_free(renderer->data);
    bj_free(renderer);
}

static bj_bool x11_init_video(
    struct bj_video_layer* layer,
    struct bj_error** error
) {
    void* handle = bj_load_library("libX11.so.6", error);
    if(handle == 0) {
        return BJ_FALSE;
    }

    x11.handle = handle;

    LOAD_SYM(x11.XAllocSizeHints,      handle, "XAllocSizeHints");
    LOAD_SYM(x11.XCreateImage,         handle, "XCreateImage");
    LOAD_SYM(x11.XCreateWindow,        handle, "XCreateWindow");
    LOAD_SYM(x11.XDeleteContext,       handle, "XDeleteContext");
    LOAD_SYM(x11.XDestroyWindow,       handle, "XDestroyWindow");
    LOAD_SYM(x11.XEventsQueued,        handle, "XEventsQueued");
    LOAD_SYM(x11.XFindContext,         handle, "XFindContext");
    LOAD_SYM(x11.XFlush,               handle, "XFlush");
    LOAD_SYM(x11.XFree,                handle, "XFree");
    LOAD_SYM(x11.XFreeGC,              handle, "XFreeGC");
    LOAD_SYM(x11.XMapWindow,           handle, "XMapWindow");
    LOAD_SYM(x11.XGetWindowAttributes, handle, "XGetWindowAttributes");
    LOAD_SYM(x11.XNextEvent,           handle, "XNextEvent");
    LOAD_SYM(x11.XPeekEvent,           handle, "XPeekEvent");
    LOAD_SYM(x11.XPending,             handle, "XPending");
    LOAD_SYM(x11.XQLength,             handle, "XQLength");
    LOAD_SYM(x11.XCreateGC,            handle, "XCreateGC");
    LOAD_SYM(x11.XPutImage,            handle, "XPutImage");
    LOAD_SYM(x11.XSaveContext,         handle, "XSaveContext");
    LOAD_SYM(x11.XSetWMNormalHints,    handle, "XSetWMNormalHints");
    LOAD_SYM(x11.XSetWMProtocols,      handle, "XSetWMProtocols");
    LOAD_SYM(x11.XStoreName,           handle, "XStoreName");
    LOAD_SYM(x11.XSync,                handle, "XSync");
    LOAD_SYM(x11.XUnmapWindow,         handle, "XUnmapWindow");

    pfn_XBlackPixel         x11_XBlackPixel;
    pfn_XDefaultDepth       x11_XDefaultDepth;
    pfn_XDefaultScreen      x11_XDefaultScreen;
    pfn_XDefaultVisual      x11_XDefaultVisual;
    pfn_XInternAtom         x11_XInternAtom;
    pfn_XOpenDisplay        x11_XOpenDisplay;
    pfn_XrmUniqueQuark      x11_XrmUniqueQuark;
    LOAD_SYM(x11_XBlackPixel,    handle, "XBlackPixel");
    LOAD_SYM(x11_XDefaultDepth,  handle, "XDefaultDepth");
    LOAD_SYM(x11_XDefaultScreen, handle, "XDefaultScreen");
    LOAD_SYM(x11_XDefaultVisual, handle, "XDefaultVisual");
    LOAD_SYM(x11_XInternAtom,    handle, "XInternAtom");
    LOAD_SYM(x11_XOpenDisplay,   handle, "XOpenDisplay");
    LOAD_SYM(x11_XrmUniqueQuark, handle, "XrmUniqueQuark");

    Display* display = x11_XOpenDisplay(0);
    if(display == 0) {
        bj_set_error(error, BJ_ERROR_INITIALIZE, "cannot open X11 display");
        return BJ_FALSE;
    }

    x11.display          = display;
    x11.default_screen   = x11_XDefaultScreen(display);
    x11.default_depth    = x11_XDefaultDepth(display, x11.default_screen);
    x11.default_visual   = x11_XDefaultVisual(display, x11.default_screen),
    x11.black_pixel      = x11_XBlackPixel(display, x11.default_screen),

    x11.wm_delete_window = x11_XInternAtom(display, "WM_DELETE_WINDOW", False);
    x11.wm_protocols     = x11_XInternAtom(display, "WM_PROTOCOLS", False);
    x11.window_context   = x11_XrmUniqueQuark();

    x11_init_keycodes(0);

    layer->create_renderer           = x11_create_renderer;
    layer->create_window             = x11_create_window;
    layer->delete_window             = x11_delete_window;
    layer->destroy_renderer          = x11_destroy_renderer;
    layer->end                       = x11_end_video;
    layer->get_window_size           = x11_get_window_size;
    layer->poll_events               = x11_poll_events;

    return BJ_TRUE;
}

struct bj_video_layer_create_info x11_video_layer_info = {
    .name   = "x11",
    .create = x11_init_video,
};

#endif
