#include <banjo/system.h>

#ifdef BJ_OS_WINDOWS

void* bj_load_library(
    const char* p_path
) {
    (void)p_path;

    return 0;
}

void bj_unload_library(
    void* p_handle
) {
    (void)p_handle;
}

void* bj_get_symbol(
    void*       p_handle,
    const char* p_name
) {
    (void)p_handle;
    (void)p_name;

    return 0;
}

#endif
