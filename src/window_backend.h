#pragma once

#include <banjo/error.h>
#include <banjo/window.h>

typedef struct bj_window_backend_data bj_window_backend_data;

typedef struct bj_window_backend_t {
    void (*dispose)(struct bj_window_backend_t*, bj_error** p_error);
    bj_window* (*create_window)(bj_window_backend_data*, const char*, u16, u16, u16, u16);
    void (*delete_window)(bj_window_backend_data*, bj_window*);
    bj_window_backend_data* p_data;
} bj_window_backend;

typedef struct {
    const char* name;
    bj_window_backend* (*create)(bj_error** p_error);
} bj_window_backend_create_info;


