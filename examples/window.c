#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/window.h>

#include <unistd.h>

#include <X11/Xlib.h>

void key_event(bj_window* p_window, bj_key_event_mode mode) {
    switch(mode) {
        case BJ_KEY_PRESSED:
            bj_info("Pressed");
            break;
        case BJ_KEY_RELEASED:
            bj_info("Released");
            bj_window_set_should_close(p_window);
            break;
        case BJ_KEY_REPEAT:
            bj_info("Repeat");
            break;

    }
}

int main(int argc, char* argv[]) {

    bj_error* p_error = 0;

    if(!bj_system_init(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 100, 100, 800, 600);

    bj_window_set_key_event(window, key_event);

    while(!bj_window_should_close(window)) {
        bj_poll_events();
    }

    bj_window_del(window);

    bj_system_dispose(0);
    return 0;
}