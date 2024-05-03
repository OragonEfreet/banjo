#pragma once

#include <banjo/array.h>
#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct bj_framebuffer_t bj_framebuffer;

struct bj_framebuffer_t {
    usize width;
    usize height;
    struct bj_array_t array;
};

BANJO_EXPORT bj_framebuffer* bj_famebuffer_init_default(
    bj_framebuffer*   p_framebuffer,
    usize             width,
    usize             height
);

BANJO_EXPORT bj_framebuffer* bj_renderer_reset(
    bj_framebuffer* p_framebuffer
);


