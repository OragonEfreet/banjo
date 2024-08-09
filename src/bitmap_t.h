#pragma once

#include <banjo/bitmap.h>

#include "dib.h"

struct bj_bitmap_t {
    usize           width;
    usize           height;
    usize           stride;
    bj_pixel_format format;
    void*           buffer;
};

bj_bitmap* bj_bitmap_init(bj_bitmap* p_bitmap, usize width, usize height, bj_pixel_format format);
bj_bitmap* bj_bitmap_init_from_buffer(bj_bitmap* p_bitmap, usize width, usize height, bj_pixel_format format, usize stride, void* buffer, int flags, bj_error** p_error);
void bj_bitmap_reset(bj_bitmap* p_bitmap);

