#include <banjo/system.h>

#ifdef BJ_OS_UNIX

#include <banjo/error.h>

#include <dlfcn.h>

void* bj_load_library(
    const char*       p_path,
    struct bj_error** error
) {
    void* handle = dlopen(p_path, RTLD_LAZY | RTLD_LOCAL);
    if (handle == 0) {
        bj_set_error_fmt(error, BJ_ERROR_SYSTEM,
                         "Cannot load library '%s': %s", p_path, dlerror());
    }
    return handle;
}

void bj_unload_library(
    void* p_handle
) {
    dlclose(p_handle);
}

void* bj_library_symbol(
    void*             p_handle,
    const char*       p_name,
    struct bj_error** error
) {
    dlerror(); // Clear any existing error
    void* symbol = dlsym(p_handle, p_name);
    const char* err = dlerror();
    if (err != 0) {
        bj_set_error_fmt(error, BJ_ERROR_SYSTEM,
                         "Cannot find symbol '%s': %s", p_name, err);
        return 0;
    }
    return symbol;
}

#endif
