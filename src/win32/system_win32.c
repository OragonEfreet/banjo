#include <banjo/system.h>

#ifdef BJ_OS_WINDOWS

#include <banjo/error.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

#include <check.h>

void* bj_load_library(
    const char*       p_path,
    struct bj_error** error
) {
    bj_check_or_0(p_path);
    HMODULE handle = LoadLibraryA(p_path);
    if (handle == 0) {
        bj_set_error_fmt(error, BJ_ERROR_SYSTEM,
                         "Cannot load library '%s' (error %lu)",
                         p_path, GetLastError());
    }
    return (void*)handle;
}

void bj_unload_library(
    void* p_handle
) {
    bj_check(p_handle);
    FreeLibrary((HMODULE)p_handle);
}

void* bj_library_symbol(
    void*             p_handle,
    const char*       p_name,
    struct bj_error** error
) {
    bj_check_or_0(p_handle);
    bj_check_or_0(p_name);
    void* symbol = (void*)GetProcAddress((HMODULE)p_handle, p_name);
    if (symbol == 0) {
        bj_set_error_fmt(error, BJ_ERROR_SYSTEM,
                         "Cannot find symbol '%s' (error %lu)",
                         p_name, GetLastError());
    }
    return symbol;
}

#endif
