////////////////////////////////////////////////////////////////////////////////
/// \example template_callbacks.c
/// Base code for using Banjo API with callback entry points
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>


int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    if(!bj_begin(BJ_VIDEO_SYSTEM | BJ_AUDIO_SYSTEM, 0)) {
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
    bj_end();
    return status;
}

