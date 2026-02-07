#ifndef BJ_RENDERER_T_H
#define BJ_RENDERER_T_H

#include <banjo/error.h>

struct bj_bitmap;
struct bj_renderer;
struct bj_renderer_data;
struct bj_window;

typedef bj_bool (*bj_renderer_configure_fn)(
    struct bj_renderer* renderer,
    struct bj_window*   window,
    struct bj_error**   error
);

typedef struct bj_bitmap* (*bj_renderer_get_framebuffer_fn)(
    struct bj_renderer* renderer
);

typedef void (*bj_renderer_present_fn)(
    struct bj_renderer* renderer,
    struct bj_window* window
);

struct bj_renderer {
    bj_renderer_configure_fn       configure;
    bj_renderer_get_framebuffer_fn get_framebuffer;
    bj_renderer_present_fn         present;

    struct bj_renderer_data* data;
};

#endif

