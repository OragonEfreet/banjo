////////////////////////////////////////////////////////////////////////////////
/// \example events.c
/// Basic event handling.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window = 0;

void cursor_event(bj_window* p_window, int x, int y) {
    bj_info("Cursor event, window %p, (%d,%d)",
        (void*)p_window, x, y
    );
}

void button_event(bj_window* p_window, int button, bj_event_action action, int x, int y) {
    bj_info("Button event, window %p, button %d, %s, (%d,%d)",
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


    bj_info("Key 0x%04X (%s) Scancode 0x%04X (with no mods) was %s", 
        key, bj_get_key_name(key), scancode, action_str
    );

    if(key == BJ_KEY_ESCAPE) {
        bj_window_set_should_close(p_window);
    }
}

void enter_event(bj_window* p_window, bj_bool enter, int x, int y) {
    bj_info("Enter event, window %p, %s, (%d,%d)",
        (void*)p_window, 
        enter ? "entered" : "left",
        x, y
    );
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600, 0);

    bj_window_set_key_event(window, key_event);
    bj_window_set_button_event(window, button_event);
    bj_window_set_cursor_event(window, cursor_event);
    bj_window_set_enter_event(window, enter_event);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_poll_events();
    bj_sleep(30);

    return bj_window_should_close(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_window_del(window);
    bj_end(0);
    return status;
}

