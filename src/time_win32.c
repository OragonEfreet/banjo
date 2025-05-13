#include <banjo/time.h>

#ifdef BJ_OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <profileapi.h>

#include "check.h"

static uint64_t s_performance_frequency = 0;

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
