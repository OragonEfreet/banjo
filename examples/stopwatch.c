////////////////////////////////////////////////////////////////////////////////
/// \example stopwatch.c
/// Code example demonstrating the use of bj_stopwatch for 3 seconds.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

static bj_stopwatch p_stopwatch = {0};

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if (!bj_begin(BJ_NO_SYSTEM, &p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_step_stopwatch(&p_stopwatch);

    double elapsed = bj_stopwatch_elapsed(&p_stopwatch);
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
