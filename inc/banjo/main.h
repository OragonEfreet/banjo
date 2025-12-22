////////////////////////////////////////////////////////////////////////////////
/// \file main.h
/// \brief Portable main substitution and application callback facilities.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup main Main Entry Point
/// \ingroup core
///
/// \brief Platform-specific main function abstraction and callback system.
///
/// This module provides a portable entry point system for applications that
/// works across different platforms (Windows, Emscripten, iOS) by wrapping
/// platform-specific initialization code.
///
/// Two usage modes are available:
/// - **Standard mode**: Define your own \c bj_main function.
/// - **Callback mode**: Implement lifecycle callbacks (begin, iterate, end).
///
/// \par Configuration Macros
///
/// - \c BJ_NO_AUTOMAIN: Define to provide your own \c main function.
/// - \c BJ_AUTOMAIN_CALLBACKS: Define to use the callback system instead of \c bj_main.
/// - \c BJ_AUTOMAIN: (Internal) Automatically defined on platforms requiring special setup.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_MAIN_H
#define BJ_MAIN_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_NO_AUTOMAIN
/// \brief Opt out of Banjo's automatic main function handling.
///
/// Define this macro before including main.h if you want to provide your own
/// \c main function instead of using \c bj_main or the callback system.
////////////////////////////////////////////////////////////////////////////////
// #define BJ_NO_AUTOMAIN 1

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUTOMAIN_CALLBACKS
/// \brief Enable the callback-based application lifecycle system.
///
/// Define this macro to use the callback system (\ref bj_app_begin,
/// \ref bj_app_iterate, \ref bj_app_end) instead of implementing \c bj_main.
////////////////////////////////////////////////////////////////////////////////
// #define BJ_AUTOMAIN_CALLBACKS

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUTOMAIN
/// \brief Internal flag indicating Banjo provides the platform's main function.
///
/// This macro is automatically defined on platforms that require special
/// initialization (Windows, Emscripten, iOS). You should not define it manually.
////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
/// \brief User-defined main function for Banjo applications.
///
/// Implement this function as your application's entry point when not using
/// the callback system. This replaces the standard \c main function.
///
/// \param argc Command-line argument count.
/// \param argv Command-line argument array.
///
/// \return Process exit code.
///
/// \note Do not implement this if using \ref BJ_AUTOMAIN_CALLBACKS.
///
/// \see bj_app_begin, bj_app_iterate, bj_app_end
////////////////////////////////////////////////////////////////////////////////
extern int bj_main(int argc, char* argv[]);

////////////////////////////////////////////////////////////////////////////////
/// \brief Internal function to invoke the user's main function.
///
/// This is called internally by Banjo's platform-specific entry point wrappers.
/// You should not call this directly.
///
/// \param argc     Command-line argument count.
/// \param argv     Command-line argument array.
/// \param function Pointer to the user's main function.
///
/// \return Process exit code.
////////////////////////////////////////////////////////////////////////////////
extern int bj_call_main(int argc, char* argv[], int (*function)(int argc, char* argv[]));
#endif

#ifdef BJ_AUTOMAIN_CALLBACKS

////////////////////////////////////////////////////////////////////////////////
/// \brief Result codes for application lifecycle callbacks.
///
/// These values indicate whether the application should continue running,
/// exit successfully, or exit with an error.
////////////////////////////////////////////////////////////////////////////////
enum bj_callback_result
{
    bj_callback_exit_error   = -1, ///< Exit with error status.
    bj_callback_exit_success =  0, ///< Exit successfully.
    bj_callback_continue     =  1, ///< Continue running (iterate again).
};

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_app_begin_fn
/// \brief Application initialization callback.
///
/// Called once at application startup to initialize resources.
///
/// \param[out] user_data Pointer to store application state.
/// \param argc           Command-line argument count.
/// \param argv           Command-line argument array.
///
/// \return A \ref bj_callback_result value.
////////////////////////////////////////////////////////////////////////////////
typedef int (*bj_app_begin_fn)(void**, int, char*[]);

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_app_iterate_fn
/// \brief Application main loop iteration callback.
///
/// Called repeatedly while the application is running. Implement your
/// per-frame logic here.
///
/// \param user_data Application state pointer from \ref bj_app_begin.
///
/// \return A \ref bj_callback_result value (continue or exit).
////////////////////////////////////////////////////////////////////////////////
typedef int (*bj_app_iterate_fn)(void*);

////////////////////////////////////////////////////////////////////////////////
/// \typedef bj_app_end_fn
/// \brief Application cleanup callback.
///
/// Called once before the application exits to release resources.
///
/// \param user_data Application state pointer from \ref bj_app_begin.
/// \param status    Exit status code.
///
/// \return Final exit code for the process.
////////////////////////////////////////////////////////////////////////////////
typedef int (*bj_app_end_fn)(void*, int);

////////////////////////////////////////////////////////////////////////////////
/// \brief User-defined initialization callback.
///
/// Implement this function when using \ref BJ_AUTOMAIN_CALLBACKS.
///
/// \param[out] user_data Pointer to store application state.
/// \param argc           Command-line argument count.
/// \param argv           Command-line argument array.
///
/// \return A \ref bj_callback_result value.
///
/// \see bj_app_iterate, bj_app_end
////////////////////////////////////////////////////////////////////////////////
extern int bj_app_begin(void** user_data, int argc, char *argv[]);

////////////////////////////////////////////////////////////////////////////////
/// \brief User-defined iteration callback.
///
/// Implement this function when using \ref BJ_AUTOMAIN_CALLBACKS.
///
/// \param user_data Application state pointer from \ref bj_app_begin.
///
/// \return A \ref bj_callback_result value (continue or exit).
///
/// \see bj_app_begin, bj_app_end
////////////////////////////////////////////////////////////////////////////////
extern int bj_app_iterate(void* user_data);

////////////////////////////////////////////////////////////////////////////////
/// \brief User-defined cleanup callback.
///
/// Implement this function when using \ref BJ_AUTOMAIN_CALLBACKS.
///
/// \param user_data Application state pointer from \ref bj_app_begin.
/// \param status    Exit status code.
///
/// \return Final exit code for the process.
///
/// \see bj_app_begin, bj_app_iterate
////////////////////////////////////////////////////////////////////////////////
extern int bj_app_end(void* user_data, int status);

////////////////////////////////////////////////////////////////////////////////
/// \brief Internal function to run the callback-based main loop.
///
/// This function is called internally by Banjo's generated main function.
/// You should not call this directly.
///
/// \param argc     Command-line argument count.
/// \param argv     Command-line argument array.
/// \param begin    Initialization callback.
/// \param iterate  Iteration callback.
/// \param end      Cleanup callback.
///
/// \return Process exit code.
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
/// \} // End of main group
////////////////////////////////////////////////////////////////////////////////

#endif

