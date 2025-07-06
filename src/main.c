#define BJ_MAIN_NOIMPL
#include <banjo/main.h>

#if !defined(BJ_AUTOMAIN)
int bj_run_app(int argc, char* argv[], bj_main_fn_t function) {
    return function(argc, argv);
}
#endif

