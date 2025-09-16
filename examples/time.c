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

    bj_error* p_error = 0;

    if(!bj_initialize(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
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
    bj_shutdown(0);
    return status;
}

