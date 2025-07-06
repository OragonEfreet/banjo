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
extern int bj_run_app(int argc, char* argv[], bj_main_fn_t function);
extern int bj_enter_app_main_callbacks(int argc, char* argv[], bj_app_begin_fn_t, bj_app_iterate_fn_t, bj_app_end_fn_t);

#ifdef __cplusplus
}
#endif

#if !defined(BJ_MAIN_HANDLED) && !defined(BJ_MAIN_NOIMPL)
    #if defined(BJ_MAIN_USE_CALLBACKS) || defined(BJ_MAIN_NEEDED) || defined(BJ_MAIN_AVAILABLE)
#      ifdef main
#           undef main
#      endif
#      ifdef BJ_MAIN_USE_CALLBACKS
#          define BJ_MAIN_CALLBACK_STANDARD 1
            int bj_main(int argc, char* argv[]) {
                return bj_enter_app_main_callbacks(argc, argv, bj_app_begin, bj_app_iterate, bj_app_end);
            }
#      endif
#      if (!defined(BJ_MAIN_USE_CALLBACKS) || defined(BJ_MAIN_CALLBACK_STANDARD))
#           if defined(BJ_OS_WINDOWS)
#               ifndef WINAPI
#                  define WINAPI __stdcall
#               endif
                typedef struct HINSTANCE__* HINSTANCE;
                typedef char* LPSTR;
                typedef wchar_t* PWSTR;
#               ifdef BJ_COMPILER_MSVC
#                   if defined(UNICODE) && UNICODE
                        int wmain(int argc, wchar_t* wargv[], wchar_t* wenvp) {
                            (void)argc; (void)wargv; (void)wenvp;
                            return bj_run_app(0, NULL, bj_main);
                        }
#                  else
                        int main(int argc, char* argv[]) {
                            (void)argc; (void)argv;
                            return bj_run_app(0, NULL, bj_main);
                        }
#                  endif
#               endif
#               ifdef __cplusplus
                    extern "C" {
#               endif
#               if defined(UNICODE) && UNICODE
                    int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR szCmdLine, int sw) {
#               else
                    int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
#               endif
                        (void)hInst; (void)hPrev; (void)szCmdLine; (void)sw;
                        return bj_run_app(0, NULL, bj_main);
                    }
#               ifdef __cplusplus
                    }
#               endif
#           else
                int main(int argc, char* argv[]) {
                    return bj_run_app(argc, argv, bj_main);
                }
#           endif
#       endif
#       define main bj_main
#   endif
#endif

#endif

