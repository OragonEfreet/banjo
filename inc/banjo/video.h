#pragma once

#include <banjo/error.h>
#include <banjo/window.h>

struct bj_video_layer_t;

typedef void (*bj_window_dispose_fn)(struct bj_video_layer_t*, bj_error** p_error);
typedef bj_window* (*bj_window_create_window_fn)(struct bj_video_layer_t*, const char*, uint16_t, uint16_t, uint16_t, uint16_t, uint8_t);
typedef void (*bj_window_delete_window_fn)(struct bj_video_layer_t*, bj_window*);
typedef void (*bj_window_poll_events_fn)(struct bj_video_layer_t*);
typedef int (*bj_window_get_size_fn)(struct bj_video_layer_t*, const bj_window*, int*, int*);
typedef bj_bitmap* (*bj_window_create_framebuffer_fn)(struct bj_video_layer_t*, const bj_window*, bj_error** p_error);
typedef void (*bj_window_flush_framebuffer_fn)(struct bj_video_layer_t*, const bj_window*);

typedef struct bj_video_layer_t {
    bj_window_dispose_fn                 dispose;
    bj_window_create_window_fn           create_window;
    bj_window_delete_window_fn           delete_window;
    bj_window_poll_events_fn             poll_events;
    bj_window_get_size_fn                get_window_size;
    bj_window_create_framebuffer_fn      create_window_framebuffer;
    bj_window_flush_framebuffer_fn       flush_window_framebuffer;

    uint64_t                             timer_base;
} bj_video_layer;

typedef struct {
    const char*          name;
    bj_video_layer* (*create)(bj_error** p_error);
} bj_video_layer_create_info;


