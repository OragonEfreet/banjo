#include <banjo/time.h>

#ifdef BJ_OS_UNIX

#include <stdint.h>
#include <time.h>

static clockid_t   s_clock;
static uint64_t    s_frequency;
static uint64_t    s_timer_base;

void bj_init_timer(
    void
) {
    s_clock = CLOCK_REALTIME;
    s_frequency = 1000000000;

#if defined(_POSIX_MONOTONIC_CLOCK)
    struct timespec spec;
    if (clock_gettime(CLOCK_MONOTONIC, &spec) == 0) {
        s_clock = CLOCK_MONOTONIC;
    }
#endif

    s_timer_base = bj_get_time_counter();
}

uint64_t bj_get_time_counter(
    void
) {
    struct timespec spec;
    clock_gettime(s_clock, &spec);
    return (uint64_t) spec.tv_sec * s_frequency + (uint64_t) spec.tv_nsec;
}

uint64_t bj_get_time_frequency(
    void
) {
    return s_frequency;
}

void bj_sleep(
    int milliseconds
) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

double bj_get_time(
    void
) {
    return (double)(bj_get_time_counter() - s_timer_base) / bj_get_time_frequency();
}

#endif
