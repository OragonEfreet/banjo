#include <banjo/system.h>

#ifdef BJ_OS_UNIX

#include <dlfcn.h> 
#include <stdint.h>
#include <time.h>

static clockid_t   s_clock;
static uint64_t    s_frequency;

void* bj_load_library(
    const char* p_path
) {
    return dlopen(p_path, RTLD_LAZY | RTLD_LOCAL);
}

void bj_unload_library(
    void* p_handle
) {
    dlclose(p_handle);
}

void* bj_get_symbol(
    void*       p_handle,
    const char* p_name
) {
    return dlsym(p_handle, p_name);
}

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

#endif
