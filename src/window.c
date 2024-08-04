#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/window.h>

#include "window_backend.h"

extern bj_window_backend_create_info x11_backend_create_info;
extern bj_window_backend_create_info fake_backend_create_info;

static const bj_window_backend_create_info* backend_create_infos[] = {
#ifdef BANJO_HAVE_X11
    &x11_backend_create_info,
#endif
    &fake_backend_create_info,
};

static bj_window_backend* s_backend = 0;

#define FORCED_BACKEND 0

void system_init_window(
    bj_error** p_error
) {
    // TODO Smarter Backend selection
    // For now, the backend is forced by taking the first available.
    // What I want is the possibility to test each backend initialization in
    // order of favorited, and return on the first that worked.
    const bj_window_backend_create_info* p_create_info = backend_create_infos[FORCED_BACKEND];
    s_backend = p_create_info->create(p_error);
    bj_info("Initialized %s window backend", p_create_info->name);
}

void system_dispose_window(
    bj_error** p_error
) {
    void (*dispose)(struct bj_window_backend_t*, bj_error** p_error) = s_backend->dispose;
    dispose(s_backend, p_error);
    bj_info("Disposed window backend");
}

bj_window* bj_window_new(
    const char* p_title,
    u16 x,
    u16 y,
    u16 width,
    u16 height
) {
    bj_trace("Creating Window");
    return s_backend->create_window(s_backend, p_title, x, y, width, height);
}

void bj_window_del(
    bj_window* p_window
) {
    bj_trace("Deleting Window");
    s_backend->delete_window(s_backend, p_window);
}
