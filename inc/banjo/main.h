////////////////////////////////////////////////////////////////////////////////
/// \file main.h
///
#pragma once

#include <banjo/api.h>

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Platform pre-configuration
//
#if defined(BJ_OS_WINDOWS)
#   define BJ_MAIN_REDEFINED
#endif


////////////////////////////////////////////////////////////////////////////////
// Callbacks API
#ifdef BJ_MAIN_USE_CALLBACKS

/// \brief Return codes determining how to handle callback returns
typedef enum bj_callback_result_t
{
    bj_callback_exit_error   = -1, //!< Application must quit with `A3D_ERROR`
    bj_callback_exit_success =  0, //!< Application must quit with `A3D_SUCCESS`
    bj_callback_continue     =  1, //!< Application must continue
} bj_callback_result;

/// \brief Callback function for application initialization.
///
/// When BJ_MAIN_USE_CALLBACKS is defined, this function **must** be defined 
/// by the application code to provide startup code.
///
/// The workflow of the application is determined by the int value returned by
/// bj_app_begin:
/// - If bj_app_begin returns `bj_callback_continue`, or any positive integer 
///   value, the initialization is interpreted as successful and the application
///   hands over to bj_app_iterate.
/// - If bj_app_begin returns `bj_callback_exit_error`, or any
///   negative integer value, the initialization is interpreted as 
///   failing and the application hands over to bj_app_end.
/// - If bj_app_begin returns `bj_callback_exit_success` or _0_,
///   the initialization is interpreted as sucessful and the application 
///   hands over to bj_app_end.
///
/// \param user_data Location to custom user data, passed to \ref bj_app_iterate.
/// \param argc Number of arguments provided from command line (length of argv)
/// \param argv Array of arguments in UTF8
/// \return Any status code
///
int bj_app_begin(void** user_data, int argc, char *argv[]);

/// \brief Callback function for application main execution.
///
/// When BJ_MAIN_USE_CALLBACKS is defined, this function **must** be defined
/// by the application code to provide main runtime code.
///
/// The function is called indefinetely until it returns _0_ or a negative 
/// integer value:
/// - If bj_app_iterate returns `bj_callback_continue`, or any positive integer 
///   value, the function is called again after it returns.
/// - If bj_app_iterate returns `bj_callback_exit_error` or any
///   negative integer value, the call is interpreted as terminating in error 
///   and the application hands over to bj_app_end.
/// - If bj_app_iterate returns `bj_callback_exit_success` or _0_,
///   the call is interpreted as terminating sucessfully and the application 
///   hands over to bj_app_end.
///
///   \param user_data Custom user data pointer, set by \ref bj_app_begin
///   \return Any status code
int bj_app_iterate(void* user_data);


/// \brief Callback function for application termination
///
/// When BJ_MAIN_USE_CALLBACKS is defined, this function **must** be defined
/// by the application code to provide termination code.
///
/// After the function returns, HOOPS Exchange is terminated and unloaded.
///
/// \param user_data Custom user data pointer, set by \ref bj_app_begin
/// \param status The last status returned by either `bj_app_begin` or `bj_app_iterate`.
/// \return The value to return to operating system.
int bj_app_end(void* user_data, int status);


int bj_main(int argc, char* argv[]) {
    void* user_data = 0;
    int status = bj_app_begin(&user_data, argc, argv);
    while (status > 0) {
        status = bj_app_iterate(user_data);
    }
    return bj_app_end(user_data, status);
}
#endif // BJ_MAIN_USE_CALLBACKS

// App-supplied function for program entry.
//
// App do not directly create this function.
// Instead, they create a standard ANSI-C main function.
//
// Application should include main.h in the same file as their main function,
// and they should not use that sytmbol for anything else in that file, as
// it might get redefined.
// int bj_main(int argc, char* argv[]);
#if defined(BJ_MAIN_USE_CALLBACKS) || defined(BJ_MAIN_REDEFINED)

// Function pointer typedef for a standard entry point.
// It's the type of the function supplied by the app, may it be the entry point
// or not.
int bj_main(int argc, char* argv[]);
typedef int (*bj_main_fn_t)(int argc, char *argv[]);

// Entry Point Implementation
# ifdef BJ_OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
// Includes are order-dependend (window.h must be before)
# include <assert.h>
# include <windows.h>
# include <shellapi.h>
# include <wchar.h> // wchar_t
# undef WIN32_LEAN_AND_MEAN 

#  if defined(_MSC_VER)

// Function called by all Windows entry points.
// Get and converts the command line arguments, and forward
// to bj_main.
static int windows_run() {
    int argc = 0;
    LPWSTR* argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
    assert(argvw != NULL);

    char** argv = (char**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (argc + 1) * sizeof(*argv));
    assert(argv != NULL);
    int i;
    for (i = 0; i < argc; ++i) {
        const int utf8_size = WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, NULL, 0, NULL, NULL);
        assert(utf8_size > 0);

        argv[i] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, utf8_size);
        assert(argv[i] > NULL);

        const int bytes_written = WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, argv[i], utf8_size, NULL, NULL);
        assert(bytes_written > 0);
    }
    argv[i] = NULL;
    LocalFree(argvw);
    const int main_res = bj_main(argc, argv);
    for (i = 0; i < argc; ++i) {
        HeapFree(GetProcessHeap(), 0, argv[i]);
    }
    HeapFree(GetProcessHeap(), 0, argv);
    return main_res;
}

// Entry Point definitions for Windows Console application
#   if defined(UNICODE) && UNICODE
int wmain(int _argc, wchar_t* _wargv[], wchar_t *_wenvp) {
    (void)_wargv; (void)_wenvp;
#   else
int main(int _argc, char* _argv[]) {
    (void)_argv;
#   endif
    (void)_argc;
    return windows_run();
}
#  endif // _MSC_VER

// Entry Point definition for Win32 applications
#  ifdef __cplusplus
extern "C" {
#  endif
#  if defined(UNICODE) && UNICODE
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR szCmdLine, int sw)
#  else
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
#  endif
{
    (void)hInst; (void)hPrev; (void)szCmdLine; (void)sw;
    return windows_run();
}
#  ifdef __cplusplus
} // extern "C"
#  endif

# else // !BJ_OS_WINDOWS
int main(int argc, char *argv[]) {
    return bj_main(argc, argv);
}
# endif // !BJ_OS_WINDOWS


// Rename main to be bj_main
# define main bj_main
#endif



