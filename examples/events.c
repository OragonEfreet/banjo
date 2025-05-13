////////////////////////////////////////////////////////////////////////////////
/// \example events.c
/// Basic event handling.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/error.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#include <stdio.h>

void cursor_event(bj_window* p_window, int x, int y) {
    printf("Cursor event, window %p, (%d,%d)\n",
        (void*)p_window, x, y
    );
}

void button_event(bj_window* p_window, int button, bj_event_action action, int x, int y) {
    printf("Button event, window %p, button %d, %s, (%d,%d)\n",
        (void*)p_window, button, 
        action == BJ_PRESS ? "pressed" : "released",
        x, y
    );
}

void key_event(bj_window* p_window, bj_event_action action, bj_key key, int scancode) {
    (void)p_window;

    const char* action_str = "pressed";
    if(action != BJ_PRESS) {
        action_str = action == BJ_RELEASE ? "released" : "repeated";
    }


    printf("Key 0x%04X (%s) Scancode 0x%04X (with no mods) was %s\n", 
        key, bj_get_key_name(key), scancode, action_str
    );

    if(key == BJ_KEY_ESCAPE) {
        bj_window_set_should_close(p_window);
    }
}

void enter_event(bj_window* p_window, bool enter, int x, int y) {
    printf("Enter event, window %p, %s, (%d,%d)\n",
        (void*)p_window, 
        enter ? "entered" : "left",
        x, y
    );
}

int main(void) {

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600, 0);

    bj_window_set_key_event(window, key_event);
    bj_window_set_button_event(window, button_event);
    bj_window_set_cursor_event(window, cursor_event);
    bj_window_set_enter_event(window, enter_event);

    while(!bj_window_should_close(window)) {
        bj_poll_events();
        bj_sleep(30); 
    }

    bj_window_del(window);

    bj_end(0);
    return 0;
}
