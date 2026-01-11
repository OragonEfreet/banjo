////////////////////////////////////////////////////////////////////////////////
/// \example stopwatch.c
/// Using stopwatch for precise timing and delta time measurements.
///
/// A stopwatch tracks both total elapsed time and delta time (time since last
/// measurement). This is essential for frame-independent game logic, animation,
/// and performance profiling.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

// A stopwatch must be zero-initialized before use.
static bj_stopwatch p_stopwatch = {0};

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    if (!bj_begin(BJ_NO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    // Update the stopwatch. This must be called once per iteration to record
    // the current time. The first call starts the stopwatch.
    bj_step_stopwatch(&p_stopwatch);

    // Get total time elapsed since the stopwatch started (first step call).
    double elapsed = bj_stopwatch_elapsed(&p_stopwatch);

    // Get delta time - time since the previous step. This is crucial for
    // frame-independent movement and animation. For example:
    //   position += velocity * delay;
    // ensures objects move at constant speed regardless of frame rate.
    double delay   = bj_stopwatch_delay(&p_stopwatch);

    bj_trace("Elapsed: %.3lf s | Delay: %.3lf s", elapsed, delay);

    bj_sleep(300);
    return elapsed >= 3.0 ? bj_callback_exit_success : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_end();
    return status;
}
