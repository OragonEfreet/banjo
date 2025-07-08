#define BJ_AUTOMAIN_CALLBACKS
#define BJ_MAIN_NOIMPL
#include <banjo/main.h>

#if defined(BJ_OS_EMSCRIPTEN)
#include <emscripten.h>
#include <stdio.h>

static void* user_data                      = 0;
static bj_app_iterate_fn_t call_app_iterate = 0;
static bj_app_end_fn_t     call_app_end     = 0;

static void emscripten_main_loop(void) {
    const bj_callback_result status = call_app_iterate(user_data);
    if(status <= 0) {
        emscripten_cancel_main_loop();
        call_app_end(user_data, status);
    }
}

int bj_call_main_callbacks(
    int argc, 
    char*               argv[],
    bj_app_begin_fn_t   app_begin,
    bj_app_iterate_fn_t app_iterate,
    bj_app_end_fn_t     app_end
) {
    int status = app_begin(&user_data, argc, argv);
    if(status > 0) {
        call_app_iterate = app_iterate;
        call_app_end = app_end;
        emscripten_set_main_loop(emscripten_main_loop, 0, 0);
        return 0;
    }

    return app_end(&user_data, status);
}

#else
int bj_call_main_callbacks(
    int argc, 
    char*               argv[],
    bj_app_begin_fn_t   app_begin,
    bj_app_iterate_fn_t app_iterate,
    bj_app_end_fn_t     app_end
) {
    void* user_data = 0;
    int status = app_begin(&user_data, argc, argv);
    while (status > 0) {
        status = app_iterate(user_data);
    }
    return app_end(user_data, status);
}
#endif
