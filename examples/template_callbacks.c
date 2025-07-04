////////////////////////////////////////////////////////////////////////////////
/// \example template_callback.c
/// Base code for using Banjo API with callback entry points
////////////////////////////////////////////////////////////////////////////////
#define BJ_MAIN_USE_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>


int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_info("Hello Banjo!");
    return bj_callback_exit_success;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_end(0);
    return status;
}

