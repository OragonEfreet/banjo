////////////////////////////////////////////////////////////////////////////////
/// \example window.c
/// Creating and managing windows with event handling.
///
/// This demonstrates window creation, event processing, and proper cleanup.
/// Every graphical Banjo application needs a window and must process events
/// to remain responsive.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/window.h>

bj_window* window = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    // Create an OS window with bj_bind_window().
    // Parameters: title, x position, y position, width, height, flags
    // The last parameter (flags) can specify fullscreen, borderless, etc.
    // Returns a window handle that must be destroyed with bj_unbind_window().
    window = bj_bind_window("Simple Banjo Window", 100, 100, 800, 600, 0);

    // Set up a keyboard callback. bj_close_on_escape is a built-in helper that
    // requests window closure when ESC is pressed.
    bj_set_key_callback(bj_close_on_escape, 0);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    // Process OS events (mouse, keyboard, window resize, close button, etc).
    // This must be called every frame or the window will freeze and become
    // unresponsive. Events are either handled by callbacks or can be polled.
    bj_dispatch_events();

    // Check if the window should close. This returns true when the user clicks
    // the X button, or when a callback (like bj_close_on_escape) requests it.
    return bj_should_close_window(window)
         ? bj_callback_exit_success
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;

    // Destroy the window and free OS resources. Always call this before bj_end().
    bj_unbind_window(window);
    bj_end();
    return status;
}


