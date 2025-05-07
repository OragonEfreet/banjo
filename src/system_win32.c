#include <banjo/system.h>

#ifdef BJ_OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <profileapi.h>

static uint64_t s_performance_frequency = 0;

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

void* bj_get_symbol(
    void*       p_handle,
    const char* p_name
) {
    bj_check_or_0(p_handle);
    bj_check_or_0(p_name);
    return (void*)GetProcAddress((HMODULE)p_handle, p_name);
}

void bj_init_timer(
    void
) {
    QueryPerformanceFrequency((LARGE_INTEGER*)&s_performance_frequency);
}

uint64_t bj_get_time_counter(
    void
) {
    uint64_t result = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&result);
    return result;
}

uint64_t bj_get_time_frequency(
    void
) {
    return s_performance_frequency;
}

void bj_sleep(
    int milliseconds
) {
    Sleep(milliseconds);
}

#endif
