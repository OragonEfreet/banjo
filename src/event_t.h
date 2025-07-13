#pragma once

#include <banjo/event.h>

void bj_window_input_key(bj_window* p_window, bj_event_action action, bj_key key, int scancode);
void bj_window_input_cursor(bj_window* p_window, int x, int y);
void bj_window_input_button(bj_window* p_window, int button, bj_event_action action, int x, int y);
void bj_window_input_enter(bj_window* p_window, bj_bool enter, int x, int y);
