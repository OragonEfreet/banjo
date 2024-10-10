#pragma once

#include <banjo/window.h>

struct bj_window_t {
    bool                  must_close;
    bj_window_key_event_t p_key_event;
};

void bj_window_input_key(bj_window* p_window, bj_key_event_mode mode);




