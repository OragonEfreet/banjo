////////////////////////////////////////////////////////////////////////////////
/// \example events.c
/// Basic event handling.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/error.h>
#include <banjo/system.h>
#include <banjo/window.h>

#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    void sleep_ms(int milliseconds) {
        Sleep(milliseconds);
    }
#else
    #include <time.h>
    void sleep_ms(int milliseconds) {
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    }
#endif

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

    printf("Key 0x%04X (%s) Scancode 0x%04X (with no mods) was %s\n", 
        key, bj_get_key_name(key), scancode, action == BJ_PRESS ? "pressed" : "released"
    );

//     if(key == BJ_KEY_ESCAPE) {
//         bj_window_set_should_close(p_window);
//     }
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

    if(!bj_system_init(&p_error)) {
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 10, 10, 100, 100);

    bj_window_set_key_event(window, key_event);
    // bj_window_set_button_event(window, button_event);
    // bj_window_set_cursor_event(window, cursor_event);
    // bj_window_set_enter_event(window, enter_event);

    while(!bj_window_should_close(window)) {
        bj_poll_events();
        /* sleep_ms(300); */
    }

    bj_window_del(window);

    bj_system_dispose(0);
    return 0;
}
