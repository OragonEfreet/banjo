#ifndef BJ_RENDERER_H
#define BJ_RENDERER_H

#include <banjo/api.h>

enum bj_renderer_type {
    BJ_RENDERER_TYPE_SOFTWARE,
};

struct bj_bitmap;
struct bj_renderer;
struct bj_window;

BANJO_EXPORT struct bj_renderer* bj_create_renderer(
    enum bj_renderer_type type
);

BANJO_EXPORT void bj_destroy_renderer(
    struct bj_renderer* renderer
);

BANJO_EXPORT void bj_renderer_configure(
    struct bj_renderer* renderer,
    struct bj_window* window
);

BANJO_EXPORT struct bj_bitmap* bj_get_framebuffer(
    struct bj_renderer* renderer
);

BANJO_EXPORT void bj_present(
    struct bj_renderer* renderer,
    struct bj_window*   window
);


#endif
