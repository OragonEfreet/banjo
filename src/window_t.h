#pragma once

#include <banjo/event.h>
#include <banjo/window.h>

struct bj_window_t {
    bj_button_callback_fn_t p_button_callback;
    bj_cursor_callback_fn_t p_cursor_callback;
    bj_enter_callback_fn_t  p_enter_callback;
    // bj_key_callback_fn_t    p_key_callback;

    char                     keystates[0xFF];
    uint8_t                  flags;

    bj_bitmap*               p_framebuffer;
};




