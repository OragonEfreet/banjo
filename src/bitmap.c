#include <banjo/math.h>
#include <banjo/draw.h>

#include <bitmap_t.h>
#include <check.h>

bj_bitmap* dib_create_bitmap_from_stream(bj_stream*, bj_error**);

BANJO_EXPORT bj_bitmap* bj_allocate_bitmap(
    void
) {
    return bj_malloc(sizeof(bj_bitmap));
}

bj_bitmap* bj_init_bitmap(
    bj_bitmap*       p_bitmap,
    void*            p_pixels,
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
) {
    const size_t computed_stride = bj_compute_bitmap_stride(width, mode);
    if(stride < computed_stride) {
        stride = computed_stride;
    }

    if(p_bitmap) {
        bj_memset(p_bitmap, 0, sizeof(bj_bitmap));

        if(stride > 0) {
            size_t bufsize = stride * height;

            p_bitmap->width       = width;
            p_bitmap->height      = height;
            p_bitmap->stride      = stride;
            p_bitmap->mode        = mode;
            p_bitmap->clear_color = 0x00000000;
            p_bitmap->weak        = (p_pixels != 0);

            if (p_bitmap->weak) {
                p_bitmap->buffer = p_pixels;
            } else {
                p_bitmap->buffer = bj_malloc(bufsize);
                if (p_bitmap->buffer == 0) {
                    return 0;
                }
                bj_memset(p_bitmap->buffer, 0x00, bufsize);
            }
            
        }
    }
    return p_bitmap;
}

void bj_reset_bitmap(
    bj_bitmap* p_bitmap
) {
    bj_check(p_bitmap);

    if(p_bitmap->weak == 0) {
        bj_free(p_bitmap->buffer);
    }
    p_bitmap->buffer = 0;
    bj_destroy_bitmap(p_bitmap->charset);
}

bj_bitmap* bj_create_bitmap(
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
) {
    bj_bitmap bitmap;
    if(bj_init_bitmap(&bitmap, 0, width, height, mode, stride) == 0) {
        return 0;
    }
    bj_bitmap* p_new = bj_allocate_bitmap();
    if (p_new == 0) {
        bj_free(bitmap.buffer);
        return 0;
    }
    return bj_memcpy(p_new, &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_create_bitmap_from_pixels(
    void*            p_pixels,
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
) {
    bj_check_or_0(p_pixels);
    bj_bitmap bitmap;
    if (bj_init_bitmap(&bitmap, p_pixels, width, height, mode, stride) == 0) {
        return 0;
    }
    bj_bitmap* p_new = bj_allocate_bitmap();
    if (p_new == 0) {
        return 0;
    }
    return bj_memcpy(p_new, &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_copy_bitmap(
    const bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);
    bj_bitmap bitmap;
    if (bj_init_bitmap(&bitmap, 0, p_bitmap->width, p_bitmap->height, p_bitmap->mode, p_bitmap->stride) == 0) {
        return 0;
    }
    bj_memcpy(bitmap.buffer, p_bitmap->buffer, bitmap.stride * bitmap.height);
    bj_bitmap* p_new = bj_allocate_bitmap();
    if (p_new == 0) {
        bj_free(bitmap.buffer);
        return 0;
    }
    return bj_memcpy(p_new, &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_convert_bitmap(
    const bj_bitmap* p_src,
    bj_pixel_mode    mode
) {
    bj_check_or_0(p_src);
    bj_check_or_0(mode);

    if (p_src->mode == mode) {
        return bj_copy_bitmap(p_src);
    }
    bj_bitmap dst;
    if (bj_init_bitmap(&dst, 0, p_src->width, p_src->height, p_src->mode, p_src->stride) == 0) {
        return 0;
    }
    
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    for (size_t y = 0; y < dst.height; ++y) {
        for (size_t x = 0; x < dst.width; ++x) {
            bj_make_bitmap_rgb(p_src, x, y, &r, &g, &b);
            bj_put_pixel(&dst, x, y, bj_make_bitmap_pixel(&dst, r, g, b));
        }
    }

    bj_bitmap* p_new = bj_allocate_bitmap();
    if (p_new == 0) {
        bj_free(dst.buffer);
        return 0;
    }
    return bj_memcpy(p_new, &dst, sizeof(bj_bitmap));
}

void bj_destroy_bitmap(
    bj_bitmap*     p_bitmap
) {
    bj_reset_bitmap(p_bitmap);
    bj_free(p_bitmap);
}

size_t bj_bitmap_width(
    const bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->width;
}

size_t bj_bitmap_height(
    const bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->height;
}

int bj_bitmap_mode( 
    bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->mode;
}

size_t bj_bitmap_stride( 
    bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->stride;
}


void* bj_bitmap_pixels(
    bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->buffer;
}

uint32_t bj_make_bitmap_pixel(
    bj_bitmap* p_bitmap,
    uint8_t red,
    uint8_t green,
    uint8_t blue
) {
    bj_check_or_0(p_bitmap);
    return bj_get_pixel_value(p_bitmap->mode, red, green, blue);
}

// TODO this code is potentially underperformant.
// Doesn't take alignment into account, just uses bytes memcpy
static void buffer_set_pixel(size_t x, size_t y, size_t stride, void* buffer, int32_t value, size_t bpp) {
    const size_t   bit_offset        = y * stride * 8 + x * bpp;
    const size_t   byte_offset       = bit_offset / 8;
    const size_t   bit_in_first_byte = bit_offset % 8;
    const uint32_t aligned_value     = value << bit_in_first_byte;
    const size_t   bytes_to_copy     = (bit_in_first_byte + bpp + 7) / 8;
    bj_memcpy((uint8_t*)buffer + byte_offset, &aligned_value, bytes_to_copy);
}

static uint32_t buffer_get_pixel(size_t x, size_t y, size_t stride, void* buffer, size_t bpp) {
    const size_t bit_offset = y * stride * 8 + x * bpp;
    const size_t byte_offset = bit_offset / 8;
    const size_t bit_in_first_byte = bit_offset % 8;
    size_t bytes_to_copy = (bit_in_first_byte + bpp + 7) / 8;
    uint32_t pixel_value = 0;
    bj_memcpy(&pixel_value, (uint8_t*)buffer + byte_offset, bytes_to_copy);
    pixel_value >>= bit_in_first_byte;
    //pixel_value &= (1u << bpp) - 1; // TODO This does not work
    return pixel_value;
}

void bj_put_pixel(
    bj_bitmap* p_bitmap,
    size_t x,
    size_t y,
    uint32_t pixel
) {
    bj_check(p_bitmap);
    bj_check(x < p_bitmap->width && y < p_bitmap->height);
    buffer_set_pixel(x, y, p_bitmap->stride, p_bitmap->buffer, pixel, BJ_PIXEL_GET_BPP(p_bitmap->mode));
}

uint32_t bj_bitmap_pixel(
    const bj_bitmap* p_bitmap,
    size_t           x,
    size_t           y
) {
    bj_check_or_0(p_bitmap);
    bj_check_or_0(x < p_bitmap->width && y < p_bitmap->height);
    return buffer_get_pixel(x, y, p_bitmap->stride, p_bitmap->buffer, BJ_PIXEL_GET_BPP(p_bitmap->mode));
}

bj_bitmap* bj_create_bitmap_from_file(
    const char*       p_path,
    bj_error**        p_error
) {
    bj_error* p_inner_error = 0;

    bj_stream* p_stream = bj_open_stream_file(p_path, &p_inner_error);
    if(p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
        return 0;
    }

    bj_bitmap* p_bitmap = dib_create_bitmap_from_stream(p_stream, p_error);
    bj_close_stream(p_stream);
    return p_bitmap;
}

void bj_clear_bitmap(bj_bitmap* p_bitmap) {
    bj_check(p_bitmap);

    bj_draw_line(p_bitmap, 0, 0, p_bitmap->width - 1, 0, p_bitmap->clear_color);
    if (p_bitmap->height > 1) {
        void* first_row = p_bitmap->buffer;
        for (size_t y = 1; y < p_bitmap->height; ++y) {
            void* dest_row = (uint8_t*)p_bitmap->buffer + (p_bitmap->stride * y);
            bj_memcpy(dest_row, first_row, p_bitmap->stride);
        }
    }
}


void bj_set_bitmap_clear_color(
    bj_bitmap* p_bitmap,
    uint32_t clear_color
) {
    bj_check(p_bitmap);
    p_bitmap->clear_color = clear_color;
}



void bj_make_bitmap_rgb(
    const bj_bitmap* p_bitmap,
    size_t           x,
    size_t           y,
    uint8_t*         p_red,
    uint8_t*         p_green,
    uint8_t*         p_blue
) {
    bj_check(p_bitmap);
    bj_make_pixel_rgb(
        p_bitmap->mode,
        bj_bitmap_pixel(p_bitmap, x, y),
        p_red,
        p_green,
        p_blue
    );
}


