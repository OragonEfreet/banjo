#pragma once

#include <banjo/error.h>
#include <banjo/window.h>

typedef struct bj_window_backend_t {
    void (*dispose)(struct bj_window_backend_t*, bj_error** p_error);
    bj_window* (*create_window)(struct bj_window_backend_t*, const char*, uint16_t, uint16_t, uint16_t, uint16_t);
    void (*delete_window)(struct bj_window_backend_t*, bj_window*);
} bj_window_backend;

typedef struct {
    const char* name;
    bj_window_backend* (*create)(bj_error** p_error);
} bj_window_backend_create_info;


