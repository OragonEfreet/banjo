#pragma once

#include <banjo/api.h>

#include <stdio.h>

#if defined(BJ_OS_WINDOWS)
#   define BJ_MAIN_REDEFINED
#endif

#ifdef BJ_MAIN_USE_CALLBACKS

typedef enum bj_callback_result_t
{
    bj_callback_exit_error   = -1,
    bj_callback_exit_success =  0,
    bj_callback_continue     =  1,
} bj_callback_result;

int bj_app_begin(void** user_data, int argc, char *argv[]);
int bj_app_iterate(void* user_data);
int bj_app_end(void* user_data, int status);


int bj_main(int argc, char* argv[]) {
    void* user_data = 0;
    int status = bj_app_begin(&user_data, argc, argv);
    while (status > 0) {
        status = bj_app_iterate(user_data);
    }
    return bj_app_end(user_data, status);
}
#endif

#if defined(BJ_MAIN_USE_CALLBACKS) || defined(BJ_MAIN_REDEFINED)

int bj_main(int argc, char* argv[]);
typedef int (*bj_main_fn_t)(int argc, char *argv[]);

# ifdef BJ_OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <assert.h>
# include <windows.h>
# include <shellapi.h>
# include <wchar.h> // wchar_t
# undef WIN32_LEAN_AND_MEAN 

#  if defined(_MSC_VER)

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



