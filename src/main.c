#define HE_MAIN_NOIMPL
#include <banjo/main_new.h>

#if !defined(BJ_MAIN_NEEDED) && !defined(BJ_MAIN_AVAILABLE)

#   if defined(BJ_OS_WINDOWS)
#   define WIN32_LEAN_AND_MEAN
#   include <assert.h>
#   include <windows.h>
#   include <shellapi.h>
#   include <wchar.h> // wchar_t
#   undef WIN32_LEAN_AND_MEAN 

    int bj_run_app(int ignore_argc, char* ignore_argv[], bj_main_fn_t function) {
        (void)ignore_argc;(void)ignore_argv;
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
        const int main_res = function(argc, argv);
        for (i = 0; i < argc; ++i) {
            HeapFree(GetProcessHeap(), 0, argv[i]);
        }
        HeapFree(GetProcessHeap(), 0, argv);
        return main_res;

    }

#   else
    int bj_run_app(int argc, char* argv[], bj_main_fn_t function) {
        return function(argc, argv);
    }
#   endif

#endif

int bj_enter_app_main_callbacks(
    int argc, char* argv[],
    bj_app_begin_fn_t app_begin,
    bj_app_iterate_fn_t app_iterate,
    bj_app_end_fn_t app_end
) {
    void* user_data = 0;
    int status = app_begin(&user_data, argc, argv);
    while (status > 0) {
        status = app_iterate(user_data);
    }
    return app_end(user_data, status);
}
