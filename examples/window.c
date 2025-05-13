////////////////////////////////////////////////////////////////////////////////
/// \example window.c
/// How to open and close windows.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/window.h>

void key_event(bj_window* p_window, bj_event_action mode, bj_key key, int scancode) {
    (void)key;
    (void)p_window;
    (void)scancode;

    switch(mode) {
        case BJ_PRESS:
            bj_info("Pressed");
            break;
        case BJ_RELEASE:
            bj_info("Released");
            bj_window_set_should_close(p_window);
            break;
        case BJ_REPEAT:
            bj_info("Repeat");
            break;
    }
}

int main(void) {

    bj_error* p_error = 0;


    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600, 0);

    bj_window_set_key_event(window, key_event);

    while(!bj_window_should_close(window)) {
        bj_poll_events();
    }

    bj_window_del(window);

    bj_end(0);
    return 0;
}
