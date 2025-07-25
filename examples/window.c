////////////////////////////////////////////////////////////////////////////////
/// \example window.c
/// How to open and close windows.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/window.h>

bj_window* window = 0;

void key_callback(bj_window* p_window, const bj_key_event* e) {
    (void)p_window;

    switch(e->action) {
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

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600, 0);
    bj_set_key_callback(key_callback);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

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


