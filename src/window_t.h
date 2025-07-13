#pragma once

#include <banjo/event.h>
#include <banjo/window.h>

struct bj_window_t {
    bj_button_event_t p_button_event;
    bj_cursor_event_t p_cursor_event;
    bj_enter_event_t  p_enter_event;
    bj_key_event_t    p_key_event;

    char                     keystates[0xFF];
    uint8_t                  flags;

    bj_bitmap*               p_framebuffer;
};




