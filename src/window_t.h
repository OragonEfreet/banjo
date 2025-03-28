#pragma once

#include <banjo/event.h>
#include <banjo/window.h>

struct bj_window_t {
    bool                     must_close;
    bj_window_button_event_t p_button_event;
    bj_window_cursor_event_t p_cursor_event;
    bj_window_enter_event_t  p_enter_event;
    bj_window_key_event_t    p_key_event;
};

void bj_window_input_key(bj_window* p_window, bj_event_action action, unsigned int keycode);
void bj_window_input_cursor(bj_window* p_window, int x, int y);
void bj_window_input_button(bj_window* p_window, int button, bj_event_action action, int x, int y);
void bj_window_input_enter(bj_window* p_window, bool enter, int x, int y);



