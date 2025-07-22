////////////////////////////////////////////////////////////////////////////////
/// \example events.c
/// Basic event handling.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/error.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window = 0;

void cursor_callback(bj_window* p_window, const bj_cursor_event* e) {
    bj_info("Cursor event, window %p, (%d,%d)",
        (void*)p_window, e->x, e->y
    );
}

void button_callback(bj_window* p_window, const bj_button_event* e) {
    bj_info("Button event, window %p, button %d, %s, (%d,%d)",
        (void*)p_window, e->button, 
        e->action == BJ_PRESS ? "pressed" : "released",
        e->x, e->y
    );
}

void key_callback(bj_window* p_window, const bj_key_event* e) {
    (void)p_window;

    const char* action_str = "pressed";
    if(e->action != BJ_PRESS) {
        action_str = e->action == BJ_RELEASE ? "released" : "repeated";
    }

    bj_info("Key 0x%04X (%s) Scancode 0x%04X (with no mods) was %s", 
        e->key, bj_get_key_name(e->key), e->scancode, action_str
    );

    if(e->key == BJ_KEY_ESCAPE) {
        bj_window_set_should_close(p_window);
    }
}

void enter_callback(bj_window* p_window, const bj_enter_event* e) {
    bj_info("Enter event, window %p, %s, (%d,%d)",
        (void*)p_window, 
        e->enter ? "entered" : "left",
        e->x, e->y
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

    bj_set_key_callback(key_callback);
    bj_set_button_callback(button_callback);
    bj_set_cursor_callback(cursor_callback);
    bj_set_enter_callback(enter_callback);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
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

