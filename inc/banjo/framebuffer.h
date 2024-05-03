#pragma once

#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/color.h>

typedef struct bj_framebuffer_t bj_framebuffer;

typedef usize bj_pixel[2];

struct bj_framebuffer_t {
    usize width;
    usize height;
    bj_color* buffer;
};

BANJO_EXPORT bj_framebuffer* bj_framebuffer_init_default(
    bj_framebuffer*   p_framebuffer,
    usize             width,
    usize             height
);

BANJO_EXPORT bj_framebuffer* bj_renderer_reset(
    bj_framebuffer* p_framebuffer
);

BANJO_EXPORT void bj_framebuffer_clear(
    bj_framebuffer* p_framebuffer,
    bj_color clear_color
);

BANJO_EXPORT void* bj_framebuffer_data(
    bj_framebuffer* p_framebuffer
);

BANJO_EXPORT void bj_framebuffer_put(
    bj_framebuffer* p_framebuffer,
    usize x,
    usize y,
    bj_color color
);

BANJO_EXPORT bj_color bj_framebuffer_get(
    bj_framebuffer* p_framebuffer,
    usize x,
    usize y
);

BANJO_EXPORT void bj_framebuffer_draw_line(
    bj_framebuffer* fb,
    bj_pixel p0,
    bj_pixel p1,
    bj_color c
);

BANJO_EXPORT void bj_framebuffer_draw_triangle(
    bj_framebuffer* fb,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    bj_color c
);

