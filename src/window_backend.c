#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "window_backend.h"
#include "window_t.h"

static bj_window* fake_window_new(
    bj_window_backend* p_backend,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
) {
    bj_window* window = bj_malloc(sizeof(bj_window));
    window->must_close = true;
    return window;
}

static void fake_window_del(
    bj_window_backend* p_backend,
    bj_window* p_window
) {
    bj_free(p_window);
}

static void fake_dispose_backend(
    bj_window_backend* p_backend,
    bj_error** p_error
) {
    bj_free(p_backend);
}

static void fake_window_poll(
    bj_window_backend* p_backend
) {
    // EMPTY
}

static bj_window_backend* fake_init_backend(
    bj_error** p_error
) {
    bj_window_backend* p_backend = bj_malloc(sizeof(bj_window_backend));
    p_backend->dispose           = fake_dispose_backend;
    p_backend->create_window     = fake_window_new;
    p_backend->delete_window     = fake_window_del;
    p_backend->poll_events       = fake_window_poll;
    return p_backend;
}

bj_window_backend_create_info fake_backend_create_info = {
    .name = "Fake",
    .create = fake_init_backend,
};

