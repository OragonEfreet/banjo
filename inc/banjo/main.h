////////////////////////////////////////////////////////////////////////////////
/// \file main.h
/// Portable main substitution and application callback facilities
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_MAIN_H
#define BJ_MAIN_H

#include <banjo/api.h>

// Provided by the user to say that the client application is
// provided its own main implementation.
// #define BJ_NO_AUTOMAIN 1

// Provided by the user to say that the client wants to use the
// callback system.
// #define BJ_AUTOMAIN_CALLBACKS

// Defined internally to say that BJ will rename main() to bj_main
// in order to provide a special main.
// #define BJ_AUTOMAIN


// Determining if main function is provided, optionnaly or not
#ifndef BJ_NO_AUTOMAIN
#   if defined(BJ_OS_EMSCRIPTEN)
#       define BJ_AUTOMAIN
#   elif defined(BJ_OS_WINDOWS)
#       define BJ_AUTOMAIN
#   elif defined(BJ_OS_IOS)
#       define BJ_AUTOMAIN
#   endif
#endif

// // Rename user's main function
// #if defined(BJ_AUTOMAIN) || defined(BJ_AUTOMAIN_CALLBACKS)
// #   define main bj_main
// #endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BJ_AUTOMAIN) || defined(BJ_AUTOMAIN_CALLBACKS)
extern int bj_main(int argc, char* argv[]);
extern int bj_call_main(int argc, char* argv[], int (*function)(int argc, char* argv[]));
#endif

#ifdef BJ_AUTOMAIN_CALLBACKS

enum bj_callback_result
{
    bj_callback_exit_error   = -1,
    bj_callback_exit_success =  0,
    bj_callback_continue     =  1,
};

typedef int (*bj_app_begin_fn)(void**, int, char*[]);
typedef int (*bj_app_iterate_fn)(void*);
typedef int (*bj_app_end_fn)(void*, int);

extern int bj_app_begin(void** user_data, int argc, char *argv[]);
extern int bj_app_iterate(void* user_data);
extern int bj_app_end(void* user_data, int status);

extern int bj_call_main_callbacks(int argc, char* argv[], bj_app_begin_fn, bj_app_iterate_fn, bj_app_end_fn);

#endif


#ifdef __cplusplus
}
#endif

#if !defined(BJ_NO_AUTOMAIN) && !defined(BJ_MAIN_NOIMPL)
    #if defined(BJ_AUTOMAIN_CALLBACKS) || defined(BJ_AUTOMAIN)
#      ifdef main
#           undef main
#      endif
#      ifdef BJ_AUTOMAIN_CALLBACKS
#          define BJ_MAIN_CALLBACK_STANDARD 1
            int bj_main(int argc, char* argv[]) {
                return bj_call_main_callbacks(argc, argv, bj_app_begin, bj_app_iterate, bj_app_end);
            }
#      endif
#      if (!defined(BJ_AUTOMAIN_CALLBACKS) || defined(BJ_MAIN_CALLBACK_STANDARD))
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
                            return bj_call_main(0, NULL, bj_main);
                        }
#                  else
                        int main(int argc, char* argv[]) {
                            (void)argc; (void)argv;
                            return bj_call_main(0, NULL, bj_main);
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
                        return bj_call_main(0, NULL, bj_main);
                    }
#               ifdef __cplusplus
                    }
#               endif
#           else
                int main(int argc, char* argv[]) {
                    return bj_call_main(argc, argv, bj_main);
                }
#           endif
#       endif
#       define main bj_main
#   endif
#endif

#endif

