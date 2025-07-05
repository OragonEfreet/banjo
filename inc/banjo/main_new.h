#ifndef BJ_MAIN_H
#define BJ_MAIN_H

#include <banjo/api.h>

// Provided by the user to say that the client application is
// provided its own main implementation.
// #define BJ_MAIN_HANDLED 1

// Provided by the user to say that the client wants to use the
// callback system.
// #define BJ_MAIN_USE_CALLBACKS

// Defined internally to say that BJ will rename main() to bj_main
// in order to provide a special main.
// #define BJ_MAIN_AVAILABLE

// Defined internally to say that the platform we're compiling in
// NEEDS a special main
// #define BJ_MAIN_NEEDED

// Determining if main function is provided, optionnaly or not
#ifndef BJ_MAIN_HANDLED
#   if defined(BJ_OS_WINDOWS)
#       define BJ_MAIN_AVAILABLE
#   elif defined(BJ_OS_IOS)
#       define BJ_MAIN_NEEDED
#   endif
#endif

// Rename user's main function
#if defined(BJ_MAIN_NEEDED) || defined(BJ_MAIN_AVAILABLE) || defined(BJ_MAIN_USE_CALLBACKS)
#   define main bj_main
#endif

#include <banjo/main_callbacks.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BJ_MAIN_USE_CALLBACKS

extern bj_callback_result bj_app_begin(void** user_data, int argc, char *argv[]);
extern bj_callback_result bj_app_iterate(void* user_data);
extern bj_callback_result bj_app_end(void* user_data, int status);

#endif

typedef int (*bj_main_fn_t)(int argc, char *argv[]);

extern int bj_main(int argc, char* argv[]);
extern void bj_set_main_ready(void); // I still don't know what it does
extern int bj_run_app(int argc, char* argv[], bj_main_fn_t function);
extern int bj_enter_app_main_callbacks(int argc, char* argv[], bj_app_begin_fn_t, bj_app_iterate_fn_t, bj_app_end_fn_t);

#ifdef __cplusplus
}
#endif

#if !defined(BJ_MAIN_HANDLED) && !defined(BJ_MAIN_NOIMPL)
    #if defined(BJ_MAIN_USE_CALLBACKS) || defined(BJ_MAIN_NEEDED) || defined(BJ_MAIN_AVAILABLE)
        #include <banjo/main_impl.h>
    #endif
#endif

#endif

