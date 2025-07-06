#define BJ_MAIN_NOIMPL
#include <banjo/main.h>

#if !defined(BJ_AUTOMAIN)
int bj_run_app(int argc, char* argv[], bj_main_fn_t function) {
    return function(argc, argv);
}
#else

#if defined(BJ_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <windows.h>
#include <shellapi.h>
#include <wchar.h> // wchar_t
#undef WIN32_LEAN_AND_MEAN 

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
#endif // BJ_OS_WINDOWS

#endif

