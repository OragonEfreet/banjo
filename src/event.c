#include <banjo/assert.h>
#include <banjo/event.h>
#include <banjo/video.h>

#include <check.h>
#include <window_t.h>

extern bj_video_layer* s_video;

// Event queue implemented as a ring buffer
#define BJ_EVQ_CAP ((size_t)64)

static struct evq_t {
    bj_event array[BJ_EVQ_CAP];
    size_t   read;
    size_t   write;
} evq = { { {0} }, 0, 0 };

static struct event_callbacks_t {
    struct {
        bj_button_callback_fn_t p_call;
        void*                   p_data;
    } button;

    struct {
        bj_cursor_callback_fn_t p_call;
        void*                   p_data;
    } cursor;

    struct {
        bj_enter_callback_fn_t p_call;
        void*                  p_data;
    } enter;

    struct {
        bj_key_callback_fn_t p_call;
        void*                p_data;
    } key;

} event_callbacks;

#define evq_write(at, val) bj_memcpy(evq.array + at, val, sizeof(bj_event))
#define evq_read(at, out) bj_memcpy(out, evq.array + at, sizeof(bj_event))

// Both solution are presented by Juho Snellman.
// See https://www.snellman.net/blog/archive/2016-12-13-ring-buffers/
#define evq_mask(val) (val & (BJ_EVQ_CAP - 1))
#define evq_reset(val) evq.read = evq.write = 0
#define EVQ_UNMASKED_INDICES
#ifdef EVQ_UNMASKED_INDICES
#   define evq_push(val) bj_assert(!evq_full()); evq_write(evq_mask(evq.write++), val)
#   define evq_shift(out) bj_assert(!evq_empty()); evq_read(evq_mask(evq.read++), out)
#   define evq_empty() (evq.read == evq.write)
#   define evq_full() (evq_size() == BJ_EVQ_CAP)
#   define evq_size() (evq.write - evq.read)
#else
#   define evq_inc(index) evq_mask(index + 1)
#   define evq_push(val) bj_assert(!evq_full()); evq_write(evq.write, val); evq.write = evq_inc(evq.write)
#   define evq_shift(out) bj_assert(!evq_empty()); evq_read(evq.read, out); evq.read = evq_inc(evq.read)
#   define evq_empty() (evq.read == evq.write)
#   define evq_full() (evq_inc(evq.write) == evq.read)
#   define evq_size() (evq_mask(evq.write - evq.read))
#endif

void bj_begin_event(void) {
    evq_reset();
    bj_trace("bj_begin_event");
}

void bj_end_event(void) {
    bj_trace("bj_end_event");
}

static inline bj_bool get_next_event(bj_event* e) {
    if(evq_empty()) {
        return BJ_FALSE;
    }
    evq_shift(e);
    return BJ_TRUE;
}

bj_key_callback_fn_t bj_set_key_callback(
    bj_key_callback_fn_t   p_callback,
    void* p_user_data
) {
    bj_key_callback_fn_t p_replaced = event_callbacks.key.p_call;
    event_callbacks.key.p_call = p_callback;
    event_callbacks.key.p_data = p_user_data;
    return p_replaced;
}

void bj_dispatch_event(const bj_event* p_event) {
    bj_window* p_window = p_event->window;
    switch(p_event->type) {
        case BJ_EVENT_CURSOR:
            if(event_callbacks.cursor.p_call) {
                event_callbacks.cursor.p_call(p_window, &p_event->cursor, event_callbacks.cursor.p_data);
            }
            break;
        case BJ_EVENT_KEY:
            if(event_callbacks.key.p_call) {
                event_callbacks.key.p_call(p_window, &p_event->key, event_callbacks.key.p_data);
            }
            break;
        case BJ_EVENT_BUTTON:
            if(event_callbacks.button.p_call) {
                event_callbacks.button.p_call(p_window, &p_event->button, event_callbacks.button.p_data);
            }
            break;
        case BJ_EVENT_ENTER:
            if(event_callbacks.enter.p_call) {
                event_callbacks.enter.p_call(p_window, &p_event->enter, event_callbacks.enter.p_data);
            }
            break;
    }
}

bj_bool bj_poll_events(bj_event* p_event) {
    s_video->poll_events(s_video);
    return get_next_event(p_event);
}

void bj_dispatch_events(
    void
) {
    bj_event e;
    while(bj_poll_events(&e)) {
        bj_dispatch_event(&e);
    }
}

bj_cursor_callback_fn_t bj_set_cursor_callback(
    bj_cursor_callback_fn_t   p_callback,
    void* p_user_data
) {
    bj_cursor_callback_fn_t p_replaced = event_callbacks.cursor.p_call;
    event_callbacks.cursor.p_call = p_callback;
    event_callbacks.cursor.p_data = p_user_data;
    return p_replaced;
}

bj_button_callback_fn_t bj_set_button_callback(
    bj_button_callback_fn_t   p_callback,
    void* p_user_data
) {
    bj_button_callback_fn_t p_replaced = event_callbacks.button.p_call;
    event_callbacks.button.p_call = p_callback;
    event_callbacks.button.p_data = p_user_data;
    return p_replaced;
}

bj_enter_callback_fn_t bj_set_enter_callback(
    bj_enter_callback_fn_t    p_callback,
    void* p_user_data
) {
    bj_enter_callback_fn_t p_replaced = event_callbacks.enter.p_call;
    event_callbacks.enter.p_call = p_callback;
    event_callbacks.enter.p_data = p_user_data;
    return p_replaced;
}

void bj_close_on_escape(
    bj_window* p_window,
    const bj_key_event* p_event,
    void* p_user_data
) {
    (void)p_user_data;
    (void)p_window;

    if (p_event->key == BJ_KEY_ESCAPE && p_event->action == BJ_RELEASE) {
        bj_set_window_should_close(p_window);
    }
}


void bj_push_event(
    const bj_event* e
) {
    if(!evq_full()) {
        evq_push(e);
    }
}

void bj_push_cursor_event(
    bj_window* p_window,
    int x,
    int y
) {
    bj_push_event(
        &(bj_event) {
            .window = p_window,
            .type   = BJ_EVENT_CURSOR,
            .cursor = {.x=x, .y=y}
        }
    );
}

void bj_push_key_event(
    bj_window*      p_window,
    bj_event_action action,
    bj_key          key,
    int             scancode
) {
    bj_check(key >= 0x00 && key < 0xFF);
    bj_check(action == BJ_PRESS || action == BJ_RELEASE);

    if(p_window != 0) {
        char* keystate = &p_window->keystates[key];
        
        if (action == BJ_PRESS) {
            if (*keystate == BJ_PRESS) {
                if(!bj_get_window_flags(p_window, BJ_WINDOW_FLAG_KEY_REPEAT)) {
                    return;
                }
                action = BJ_REPEAT;
            } else {
                *keystate = BJ_PRESS;
            }
        } else {
            if (*keystate == BJ_RELEASE) {
                return;
            }
            *keystate = BJ_RELEASE;
        }
    }

    bj_push_event(
        &(bj_event) {
            .window = p_window,
            .type   = BJ_EVENT_KEY,
            .key    = {
                .action   = action,
                .key      = key,
                .scancode = scancode,
            }
        }
    );
}

void bj_push_button_event(
    bj_window* p_window,
    int button,
    bj_event_action action,
    int x,
    int y
) {
    bj_push_event(
        &(bj_event) {
            .window = p_window,
            .type   = BJ_EVENT_BUTTON,
            .button = {
                .action = action,
                .button = button,
                .x      = x,
                .y      = y,
            }
        }
    );
}

void bj_push_enter_event(
    bj_window* p_window,
    bj_bool enter,
    int x,
    int y
) {
    bj_push_event(
        &(bj_event) {
            .window = p_window,
            .type   = BJ_EVENT_ENTER,
            .enter  = {
                .x      = x,
                .y      = y,
                .enter  = enter,
            }
        }
    );
}

const char* bj_key_name(int key) {
    switch(key) {
        case BJ_KEY_UNKNOWN:             return "UNKNOWN";
        case BJ_KEY_LBUTTON:             return "LBUTTON";
        case BJ_KEY_RBUTTON:             return "RBUTTON";
        case BJ_KEY_CANCEL:              return "CANCEL";
        case BJ_KEY_MBUTTON:             return "MBUTTON";
        case BJ_KEY_XBUTTON1:            return "XBUTTON1";
        case BJ_KEY_XBUTTON2:            return "XBUTTON2";
        case BJ_KEY_BACK:                return "BACK";
        case BJ_KEY_TAB:                 return "TAB";
        case BJ_KEY_CLEAR:               return "CLEAR";
        case BJ_KEY_RETURN:              return "RETURN";
        case BJ_KEY_SHIFT:               return "SHIFT";
        case BJ_KEY_CONTROL:             return "CONTROL";
        case BJ_KEY_MENU:                return "MENU";
        case BJ_KEY_PAUSE:               return "PAUSE";
        case BJ_KEY_CAPITAL:             return "CAPITAL";
        case BJ_KEY_KANA:                return "KANA";
        case BJ_KEY_IME_ON:              return "IME_ON";
        case BJ_KEY_JUNJA:               return "JUNJA";
        case BJ_KEY_FINAL:               return "FINAL";
        case BJ_KEY_KANJI:               return "KANJI";
        case BJ_KEY_IME_OFF:             return "IME_OFF";
        case BJ_KEY_ESCAPE:              return "ESCAPE";
        case BJ_KEY_CONVERT:             return "CONVERT";
        case BJ_KEY_NONCONVERT:          return "NONCONVERT";
        case BJ_KEY_ACCEPT:              return "ACCEPT";
        case BJ_KEY_MODECHANGE:          return "MODECHANGE";
        case BJ_KEY_SPACE:               return "SPACE";
        case BJ_KEY_PRIOR:               return "PRIOR";
        case BJ_KEY_NEXT:                return "NEXT";
        case BJ_KEY_END:                 return "END";
        case BJ_KEY_HOME:                return "HOME";
        case BJ_KEY_LEFT:                return "LEFT";
        case BJ_KEY_UP:                  return "UP";
        case BJ_KEY_RIGHT:               return "RIGHT";
        case BJ_KEY_DOWN:                return "DOWN";
        case BJ_KEY_SELECT:              return "SELECT";
        case BJ_KEY_PRINT:               return "PRINT";
        case BJ_KEY_EXECUTE:             return "EXECUTE";
        case BJ_KEY_SNAPSHOT:            return "SNAPSHOT";
        case BJ_KEY_INSERT:              return "INSERT";
        case BJ_KEY_DELETE:              return "DELETE";
        case BJ_KEY_HELP:                return "HELP";
        case BJ_KEY_0:                   return "0";
        case BJ_KEY_1:                   return "1";
        case BJ_KEY_2:                   return "2";
        case BJ_KEY_3:                   return "3";
        case BJ_KEY_4:                   return "4";
        case BJ_KEY_5:                   return "5";
        case BJ_KEY_6:                   return "6";
        case BJ_KEY_7:                   return "7";
        case BJ_KEY_8:                   return "8";
        case BJ_KEY_9:                   return "9";
        case BJ_KEY_A:                   return "A";
        case BJ_KEY_B:                   return "B";
        case BJ_KEY_C:                   return "C";
        case BJ_KEY_D:                   return "D";
        case BJ_KEY_E:                   return "E";
        case BJ_KEY_F:                   return "F";
        case BJ_KEY_G:                   return "G";
        case BJ_KEY_H:                   return "H";
        case BJ_KEY_I:                   return "I";
        case BJ_KEY_J:                   return "J";
        case BJ_KEY_K:                   return "K";
        case BJ_KEY_L:                   return "L";
        case BJ_KEY_M:                   return "M";
        case BJ_KEY_N:                   return "N";
        case BJ_KEY_O:                   return "O";
        case BJ_KEY_P:                   return "P";
        case BJ_KEY_Q:                   return "Q";
        case BJ_KEY_R:                   return "R";
        case BJ_KEY_S:                   return "S";
        case BJ_KEY_T:                   return "T";
        case BJ_KEY_U:                   return "U";
        case BJ_KEY_V:                   return "V";
        case BJ_KEY_W:                   return "W";
        case BJ_KEY_X:                   return "X";
        case BJ_KEY_Y:                   return "Y";
        case BJ_KEY_Z:                   return "Z";
        case BJ_KEY_LWIN:                return "LWIN";
        case BJ_KEY_RWIN:                return "RWIN";
        case BJ_KEY_APPS:                return "APPS";
        case BJ_KEY_SLEEP:               return "SLEEP";
        case BJ_KEY_NUMPAD0:             return "NUMPAD0";
        case BJ_KEY_NUMPAD1:             return "NUMPAD1";
        case BJ_KEY_NUMPAD2:             return "NUMPAD2";
        case BJ_KEY_NUMPAD3:             return "NUMPAD3";
        case BJ_KEY_NUMPAD4:             return "NUMPAD4";
        case BJ_KEY_NUMPAD5:             return "NUMPAD5";
        case BJ_KEY_NUMPAD6:             return "NUMPAD6";
        case BJ_KEY_NUMPAD7:             return "NUMPAD7";
        case BJ_KEY_NUMPAD8:             return "NUMPAD8";
        case BJ_KEY_NUMPAD9:             return "NUMPAD9";
        case BJ_KEY_MULTIPLY:            return "MULTIPLY";
        case BJ_KEY_ADD:                 return "ADD";
        case BJ_KEY_SEPARATOR:           return "SEPARATOR";
        case BJ_KEY_SUBTRACT:            return "SUBTRACT";
        case BJ_KEY_DECIMAL:             return "DECIMAL";
        case BJ_KEY_DIVIDE:              return "DIVIDE";
        case BJ_KEY_F1:                  return "F1";
        case BJ_KEY_F2:                  return "F2";
        case BJ_KEY_F3:                  return "F3";
        case BJ_KEY_F4:                  return "F4";
        case BJ_KEY_F5:                  return "F5";
        case BJ_KEY_F6:                  return "F6";
        case BJ_KEY_F7:                  return "F7";
        case BJ_KEY_F8:                  return "F8";
        case BJ_KEY_F9:                  return "F9";
        case BJ_KEY_F10:                 return "F10";
        case BJ_KEY_F11:                 return "F11";
        case BJ_KEY_F12:                 return "F12";
        case BJ_KEY_F13:                 return "F13";
        case BJ_KEY_F14:                 return "F14";
        case BJ_KEY_F15:                 return "F15";
        case BJ_KEY_F16:                 return "F16";
        case BJ_KEY_F17:                 return "F17";
        case BJ_KEY_F18:                 return "F18";
        case BJ_KEY_F19:                 return "F19";
        case BJ_KEY_F20:                 return "F20";
        case BJ_KEY_F21:                 return "F21";
        case BJ_KEY_F22:                 return "F22";
        case BJ_KEY_F23:                 return "F23";
        case BJ_KEY_F24:                 return "F24";
        case BJ_KEY_NUMLOCK:             return "NUMLOCK";
        case BJ_KEY_SCROLL:              return "SCROLL";
        case BJ_KEY_LSHIFT:              return "LSHIFT";
        case BJ_KEY_RSHIFT:              return "RSHIFT";
        case BJ_KEY_LCONTROL:            return "LCONTROL";
        case BJ_KEY_RCONTROL:            return "RCONTROL";
        case BJ_KEY_LMENU:               return "LMENU";
        case BJ_KEY_RMENU:               return "RMENU";
        case BJ_KEY_BROWSER_BACK:        return "BROWSER_BACK";
        case BJ_KEY_BROWSER_FORWARD:     return "BROWSER_FORWARD";
        case BJ_KEY_BROWSER_REFRESH:     return "BROWSER_REFRESH";
        case BJ_KEY_BROWSER_STOP:        return "BROWSER_STOP";
        case BJ_KEY_BROWSER_SEARCH:      return "BROWSER_SEARCH";
        case BJ_KEY_BROWSER_FAVORITES:   return "BROWSER_FAVORITES";
        case BJ_KEY_BROWSER_HOME:        return "BROWSER_HOME";
        case BJ_KEY_VOLUME_MUTE:         return "VOLUME_MUTE";
        case BJ_KEY_VOLUME_DOWN:         return "VOLUME_DOWN";
        case BJ_KEY_VOLUME_UP:           return "VOLUME_UP";
        case BJ_KEY_MEDIA_NEXT_TRACK:    return "MEDIA_NEXT_TRACK";
        case BJ_KEY_MEDIA_PREV_TRACK:    return "MEDIA_PREV_TRACK";
        case BJ_KEY_MEDIA_STOP:          return "MEDIA_STOP";
        case BJ_KEY_MEDIA_PLAY_PAUSE:    return "MEDIA_PLAY_PAUSE";
        case BJ_KEY_LAUNCH_MAIL:         return "LAUNCH_MAIL";
        case BJ_KEY_LAUNCH_MEDIA_SELECT: return "LAUNCH_MEDIA_SELECT";
        case BJ_KEY_LAUNCH_APP1:         return "LAUNCH_APP1";
        case BJ_KEY_LAUNCH_APP2:         return "LAUNCH_APP2";
        case BJ_KEY_OEM_1:               return "OEM_1";
        case BJ_KEY_OEM_PLUS:            return "OEM_PLUS";
        case BJ_KEY_OEM_COMMA:           return "OEM_COMMA";
        case BJ_KEY_OEM_MINUS:           return "OEM_MINUS";
        case BJ_KEY_OEM_PERIOD:          return "OEM_PERIOD";
        case BJ_KEY_OEM_2:               return "OEM_2";
        case BJ_KEY_OEM_3:               return "OEM_3";
        case BJ_KEY_OEM_4:               return "OEM_4";
        case BJ_KEY_OEM_5:               return "OEM_5";
        case BJ_KEY_OEM_6:               return "OEM_6";
        case BJ_KEY_OEM_7:               return "OEM_7";
        case BJ_KEY_OEM_8:               return "OEM_8";
        case BJ_KEY_OEM_102:             return "OEM_102";
        case BJ_KEY_PROCESSKEY:          return "PROCESSKEY";
        case BJ_KEY_PACKET:              return "PACKET";
        case BJ_KEY_ATTN:                return "ATTN";
        case BJ_KEY_CRSEL:               return "CRSEL";
        case BJ_KEY_EXSEL:               return "EXSEL";
        case BJ_KEY_EREOF:               return "EREOF";
        case BJ_KEY_PLAY:                return "PLAY";
        case BJ_KEY_ZOOM:                return "ZOOM";
        case BJ_KEY_NONAME:              return "NONAME";
        case BJ_KEY_PA1:                 return "PA1";
        case BJ_KEY_OEM_CLEAR:           return "OEM_CLEAR";
    }
    return "Unknown";
}

