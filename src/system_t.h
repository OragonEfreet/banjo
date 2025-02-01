#pragma once

#include <banjo/error.h>
#include <banjo/window.h>

struct bj_system_backend_t;

typedef void (*bj_window_dispose_fn)(struct bj_system_backend_t*, bj_error** p_error);
typedef bj_window* (*bj_window_create_window_fn)(struct bj_system_backend_t*, const char*, uint16_t, uint16_t, uint16_t, uint16_t);
typedef void (*bj_window_delete_window_fn)(struct bj_system_backend_t*, bj_window*);
typedef void (*bj_window_poll_events_fn)(struct bj_system_backend_t*);

typedef struct bj_system_backend_t {
    bj_window_dispose_fn       dispose;
    bj_window_create_window_fn create_window;
    bj_window_delete_window_fn delete_window;
    bj_window_poll_events_fn   poll_events;
} bj_system_backend;

typedef struct {
    const char*          name;
    bj_system_backend* (*create)(bj_error** p_error);
} bj_system_backend_create_info;


