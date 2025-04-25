#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/window.h>

#include "check.h"
#include "system_t.h"
#include "window_t.h"

extern bj_system_backend* s_backend;

bj_window* bj_window_new(
    const char* p_title,
    uint16_t    x,
    uint16_t    y,
    uint16_t    width,
    uint16_t    height,
    uint8_t     window_flags
) {
    bj_trace("Creating Window");
    return s_backend->create_window(s_backend, p_title, x, y, width, height, window_flags);
}

void bj_window_del(
    bj_window* p_window
) {
    bj_trace("Deleting Window");
    s_backend->delete_window(s_backend, p_window);
}

bool bj_window_should_close(
    bj_window* p_window
) {
    bj_check_or_return(p_window, true);
    return p_window->must_close;
}

bj_window_key_event_t bj_window_set_key_event(
    bj_window* p_window,
    bj_window_key_event_t   p_event
) {
    bj_check_or_0(p_window);
    bj_window_key_event_t p_replaced = p_window->p_key_event;
    p_window->p_key_event = p_event;
    return p_replaced;
}

void bj_window_set_should_close(
    bj_window* p_window
) {
    bj_check(p_window);
    p_window->must_close = true;
}

BANJO_EXPORT void bj_poll_events(
    void
) {
    s_backend->poll_events(s_backend);
}

bj_window_cursor_event_t bj_window_set_cursor_event(
    bj_window*                 p_window,
    bj_window_cursor_event_t   p_event
) {
    bj_check_or_0(p_window);
    bj_window_cursor_event_t p_replaced = p_window->p_cursor_event;
    p_window->p_cursor_event = p_event;
    return p_replaced;
}

void bj_window_input_cursor(
    bj_window* p_window,
    int x,
    int y
) {
    bj_check(p_window);
    if(!!p_window->p_cursor_event) {
        p_window->p_cursor_event(p_window, x, y);
    }
}

bj_window_button_event_t bj_window_set_button_event(
    bj_window*                 p_window,
    bj_window_button_event_t   p_event
) {
    bj_check_or_0(p_window);
    bj_window_button_event_t p_replaced = p_window->p_button_event;
    p_window->p_button_event = p_event;
    return p_replaced;
}

void bj_window_input_key(
    bj_window*      p_window,
    bj_event_action action,
    bj_key          key,
    int             scancode
) {
    bj_check(p_window);
    bj_check(key >= 0x00 && key < 0xFF);
    bj_check(action == BJ_PRESS || action == BJ_RELEASE);

    char* keystate = &p_window->keystates[key];
    
    if (action == BJ_PRESS) {
        if (*keystate == BJ_PRESS) {
            if(!bj_window_get_flags(p_window, BJ_WINDOW_FLAG_KEY_REPEAT)) {
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

    if (p_window->p_key_event) {
        p_window->p_key_event(p_window, action, key, scancode);
    }
}

void bj_window_input_button(
    bj_window* p_window,
    int button,
    bj_event_action action,
    int x,
    int y
) {
    bj_check(p_window);
    if(!!p_window->p_button_event) {
        p_window->p_button_event(p_window, button, action, x, y);
    }
}

bj_window_enter_event_t bj_window_set_enter_event(
    bj_window*                 p_window,
    bj_window_enter_event_t    p_event
) {
    bj_check_or_0(p_window);
    bj_window_enter_event_t p_replaced = p_window->p_enter_event;
    p_window->p_enter_event = p_event;
    return p_replaced;
}

void bj_window_input_enter(
    bj_window* p_window,
    bool enter,
    int x,
    int y
) {
    bj_check(p_window);
    if(!!p_window->p_enter_event) {
        p_window->p_enter_event(p_window, enter, x, y);
    }
}

uint8_t bj_window_get_flags(
    bj_window* p_window,
    uint8_t    flags
) {
    bj_check_or_0(p_window);
    return (p_window->flags & flags);
}
