////////////////////////////////////////////////////////////////////////////////
/// \example time.c
/// Code example displaing time for 3 seconds.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    if(!bj_begin(BJ_NO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    } 

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    double elapsed = bj_run_time();
    bj_trace("- %lf", elapsed);

    bj_sleep(300);
    return elapsed >= 3.0 ? bj_callback_exit_success : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_end();
    return status;
}

