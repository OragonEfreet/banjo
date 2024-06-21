#include <banjo/error.h>
#include <banjo/memory.h>
#include <banjo/window.h>
#include <banjo/log.h>

#include "window_backend.h"

struct bj_window_t {
    int handle;
};

static bj_window* fake_window_new(
    bj_window_backend_data* p_data,
    const char* p_title,
    u16 x,
    u16 y,
    u16 width,
    u16 height
) {
    static bj_window window;
    return &window;
}

static void fake_window_del(
    bj_window_backend_data* p_data,
    bj_window* p_window
) {
    // EMPTY
}

static void fake_dispose_backend(
    bj_window_backend* p_backend,
    bj_error** p_error
) {
    bj_free(p_backend);
}

static bj_window_backend* fake_init_backend(
    bj_error** p_error
) {
    bj_window_backend* p_backend = bj_malloc(sizeof(bj_window_backend));
    p_backend->dispose           = fake_dispose_backend;
    p_backend->create_window     = fake_window_new;
    p_backend->delete_window     = fake_window_del;
    p_backend->p_data = 0;
    return p_backend;
}

bj_window_backend_create_info fake_backend_create_info = {
    .name = "Fake",
    .create = fake_init_backend,
};

