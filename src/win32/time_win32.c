#include <banjo/time.h>

#ifdef BJ_OS_WINDOWS


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <profileapi.h>

#include <check.h>

extern uint64_t bj__time_frequency;
static uint64_t    s_timer_base;

void bj_end_time() {
    // EMPTY
}

void bj_begin_time(
    void
) {
    QueryPerformanceFrequency((LARGE_INTEGER*)&bj__time_frequency);
    s_timer_base = bj_time_counter();
}

uint64_t bj_get_time(
    void
) {
    FILETIME ft;
    ULARGE_INTEGER li;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart  = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    const uint64_t EPOCH_DIFF = 11644473600ULL;
    return (li.QuadPart / 10000000ULL) - EPOCH_DIFF;
}

uint64_t bj_time_counter(
    void
) {
    uint64_t result = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&result);
    return result;
}

uint64_t bj_time_frequency(
    void
) {
    return bj__time_frequency;
}

void bj_sleep(
    int milliseconds
) {
    Sleep(milliseconds);
}

double bj_run_time(
    void
) {
    return (double)(bj_time_counter() - s_timer_base) / bj_time_frequency();
}

#endif
