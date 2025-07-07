#define BJ_AUTOMAIN_CALLBACKS
#define BJ_MAIN_NOIMPL
#include <banjo/main.h>

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
