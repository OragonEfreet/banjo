#include <banjo/system.h>

#ifdef BJ_OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

#include <check.h>

void* bj_load_library(
    const char* p_path
) {
    bj_check_or_0(p_path);
    return (void*)LoadLibraryA(p_path);
}

void bj_unload_library(
    void* p_handle
) {
    bj_check(p_handle);
    FreeLibrary((HMODULE)p_handle);
}

void* bj_library_symbol(
    void*       p_handle,
    const char* p_name
) {
    bj_check_or_0(p_handle);
    bj_check_or_0(p_name);
    return (void*)GetProcAddress((HMODULE)p_handle, p_name);
}

#endif
