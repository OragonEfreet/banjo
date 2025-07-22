////////////////////////////////////////////////////////////////////////////////
/// \example events.c
/// Basic event handling.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/event.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

/* void cursor_callback(bj_window* p_window, const bj_cursor_event* e) { */
/*     bj_info("Cursor event, window %p, (%d,%d)", */
/*         (void*)p_window, e->x, e->y */
/*     ); */
/* } */

/* void button_callback(bj_window* p_window, const bj_button_event* e) { */
/*     bj_info("Button event, window %p, button %d, %s, (%d,%d)", */
/*         (void*)p_window, e->button, */ 
/*         e->action == BJ_PRESS ? "pressed" : "released", */
/*         e->x, e->y */
/*     ); */
/* } */

void key_callback(bj_window* p_window, const bj_key_event* e) {
    (void)p_window;

    const char* action_str = "pressed";
    if(e->action != BJ_PRESS) {
        action_str = e->action == BJ_RELEASE ? "released" : "repeated";
    }

    bj_info("Key 0x%04X (%s) Scancode 0x%04X (with no mods) was %s", 
        e->key, bj_get_key_name(e->key), e->scancode, action_str
    );
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

    bj_set_key_callback(key_callback);
    /* bj_set_button_callback(window, button_callback); */
    /* bj_set_cursor_callback(window, cursor_callback); */
    bj_set_enter_callback(enter_callback);

    bj_push_key_event(0, BJ_PRESS, BJ_KEY_J, 42);
    bj_push_enter_event(0, BJ_TRUE, 10, 100);
    /* bj_push_enter_event(0, BJ_FALSE, 34, 43); */

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    return bj_callback_exit_success;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_end(0);
    return status;
}

