#pragma once

#include <banjo/bitmap.h>

struct bj_bitmap_t {
    size_t          width;
    size_t          height;
    size_t          stride;
    bj_pixel_mode   mode;
    uint32_t        clear_color;
    void*           buffer;
};

bj_bitmap* bj_bitmap_init_with(bj_bitmap* p_bitmap, size_t width, size_t height, bj_pixel_mode mode, size_t stride);
void bj_bitmap_reset(bj_bitmap* p_bitmap);

