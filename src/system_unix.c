#include <banjo/system.h>

#ifdef BJ_OS_UNIX

#include <dlfcn.h> 
#include <time.h>

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

void bj_sleep(
    int milliseconds
) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

#endif
