////////////////////////////////////////////////////////////////////////////////
/// \example time.c
/// Using Banjo's time functions for elapsed time tracking and sleeping.
///
/// This demonstrates bj_run_time() for measuring elapsed time and bj_sleep()
/// for pausing execution. These are essential for frame timing, animations,
/// and timed events.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    // BJ_NO_SYSTEM means we don't need video or audio - just basic functions
    // like time and logging work without any subsystem.
    if(!bj_begin(BJ_NO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    // bj_run_time() returns the number of seconds (as a double) since the
    // program started. Useful for animations, timeouts, and delta time
    // calculations.
    double elapsed = bj_run_time();
    bj_trace("- %lf", elapsed);

    // bj_sleep() pauses execution for the specified number of milliseconds.
    // This prevents busy-waiting and reduces CPU usage. Here we sleep for
    // 300ms between iterations.
    bj_sleep(300);

    // Exit after approximately 3 seconds have elapsed.
    return elapsed >= 3.0 ? bj_callback_exit_success : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_end();
    return status;
}

