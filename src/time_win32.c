#include <banjo/time.h>

#ifdef BJ_OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <profileapi.h>

#include "check.h"

static uint64_t s_performance_frequency = 0;
static uint64_t    s_timer_base;

void bj_end_time() {
    // EMPTY
}

void bj_begin_time(
    void
) {
    QueryPerformanceFrequency((LARGE_INTEGER*)&s_performance_frequency);
    s_timer_base = bj_get_time_counter();
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

// TODO merge with unix one?
double bj_get_time(
    void
) {
    return (double)(bj_get_time_counter() - s_timer_base) / bj_get_time_frequency();
}

#endif
