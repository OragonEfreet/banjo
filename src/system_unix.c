#include <banjo/system.h>

#include <dlfcn.h> 

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

