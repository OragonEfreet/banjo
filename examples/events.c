#include <banjo/error.h>
#include <banjo/system.h>
#include <banjo/window.h>

#include <stdio.h>

void cursor_event(bj_window* p_window, int x, int y) {
    printf("Cursor event, window %p, (%d,%d)\n",
        p_window, x, y
    );
}

void button_event(bj_window* p_window, int button, bj_event_action action, int x, int y) {
    printf("Button event, window %p, button %d, %s, (%d,%d)\n",
        p_window, button, 
        action == BJ_PRESS ? "pressed" : "released",
        x, y
    );
}

void key_event(bj_window* p_window, bj_event_action action, unsigned int keycode) {
    printf("Key event, window %p, keycode %u\n",
        p_window, keycode
    );
}

void enter_event(bj_window* p_window, bool enter, int x, int y) {
    printf("Enter event, window %p, %s, (%d,%d)\n",
        p_window, 
        enter ? "entered" : "left",
        x, y
    );
}

int main(int argc, char* argv[]) {

    bj_error* p_error = 0;

    if(!bj_system_init(&p_error)) {
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600);

    /* bj_window_set_cursor_event(window, cursor_event); */
    /* bj_window_set_key_event(window, key_event); */
    bj_window_set_button_event(window, button_event);
    bj_window_set_enter_event(window, enter_event);

    while(!bj_window_should_close(window)) {
        bj_poll_events();
    }

    bj_window_del(window);

    bj_system_dispose(0);
    return 0;
}
