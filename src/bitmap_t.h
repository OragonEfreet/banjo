#pragma once

#include <banjo/bitmap.h>

struct bj_bitmap_t {
    usize     width;
    usize     height;
    bj_color* buffer;
    bj_color  clear_color;
};

bj_bitmap* bj_bitmap_init(bj_bitmap* p_bitmap, usize width, usize height);
void bj_bitmap_reset(bj_bitmap* p_bitmap);

