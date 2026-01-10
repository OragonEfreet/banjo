////////////////////////////////////////////////////////////////////////////////
/// \example event_polling.c
/// Basic event handling using polling.
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

typedef struct {
    size_t cursor;
    size_t button;
    size_t key;
    size_t enter;
} event_counter;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)argc; (void)argv;

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    } 

    window = bj_bind_window("Event Polling", 100, 100, 800, 600, 0);
    *user_data = bj_calloc(sizeof(event_counter));

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    event_counter* counter = (event_counter*)user_data;
    bj_event e;

    while(bj_poll_events(&e)) {
        switch(e.type) {
            case BJ_EVENT_ENTER:
                ++counter->enter;
                bj_info("Enter event, window %p, %s, (%d,%d)",
                    window, 
                    e.enter.enter ? "entered" : "left",
                    e.enter.x, e.enter.y
                );
                break;

            case BJ_EVENT_CURSOR:
                ++counter->cursor;
                bj_info("Cursor event, window %p, (%d,%d)",
                    window, e.cursor.x, e.cursor.y
                );
                break;
            case BJ_EVENT_KEY:
                ++counter->key;

                const char* action_str = "pressed";
                if(e.key.action != BJ_PRESS) {
                    action_str = e.key.action == BJ_RELEASE ? "released" : "repeated";
                }

                bj_info("Key 0x%04X (%s) Scancode 0x%04X (with no mods) was %s", 
                    e.key.key, bj_key_name(e.key.key), e.key.scancode, action_str
                );

                if(e.key.key == BJ_KEY_ESCAPE) {
                    bj_set_window_should_close(window);
                }
                break;

            case BJ_EVENT_BUTTON:
                ++counter->button;
                bj_info("Button event, window %p, button %d, %s, (%d,%d)",
                    (void*)window, e.button.button, 
                    e.button.action == BJ_PRESS ? "pressed" : "released",
                    e.button.x, e.button.y
                );
                break;
        }
    }
    bj_sleep(30);

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {

    event_counter* counter = (event_counter*)user_data;
    bj_info("Total events: %ld cursor, %ld button, %ld key, %ld enter",
        counter->cursor, counter->button, counter->key, counter->enter
    );
    bj_free(counter);

    bj_unbind_window(window);
    bj_end();
    return status;
}

