#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/window.h>

#include "window_t.h"
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
    uint16_t    x,
    uint16_t    y,
    uint16_t    width,
    uint16_t    height
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

BANJO_EXPORT void bj_poll_events(
    void
) {
    s_backend->poll_events(s_backend);
}

bool bj_window_should_close(
    bj_window* p_window
) {
    return p_window->must_close;
}

bj_window_key_event_t bj_window_set_key_event(
    bj_window*              p_window,
    bj_window_key_event_t   p_event
) {
    bj_check_or_0(p_window);
    bj_window_key_event_t p_replaced = p_window->p_key_event;
    p_window->p_key_event = p_event;
    return p_replaced;
}

void bj_window_set_should_close(
    bj_window* p_window
) {
    bj_check(p_window);
    p_window->must_close = true;
}

void bj_window_input_key(
    bj_window* p_window,
    bj_key_event_mode mode
) {
    bj_check(p_window);
    if(!!p_window->p_key_event) {
        p_window->p_key_event(p_window, mode);
    }
}