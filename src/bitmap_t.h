#pragma once

#include <banjo/bitmap.h>

struct bj_bitmap_t {
    size_t                 width;
    size_t                 height;
    size_t                 stride;
    bj_pixel_mode          mode;
    uint32_t               clear_color;
    void*                  buffer;
    int                    weak;
    bj_bool                colorkey_enabled;
    uint32_t               colorkey;
    struct bj_bitmap_t*    charset;
};



