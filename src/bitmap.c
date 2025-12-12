#include <banjo/math.h>
#include <banjo/draw.h>

#include <bitmap_t.h>
#include <check.h>

struct bj_bitmap* dib_create_bitmap_from_stream(struct bj_stream*, struct bj_error**);

BANJO_EXPORT struct bj_bitmap* bj_allocate_bitmap(
    void
) {
    return bj_malloc(sizeof(struct bj_bitmap));
}

struct bj_bitmap* bj_init_bitmap(
    struct bj_bitmap*       bitmap,
    void*            pixels,
    size_t           width,
    size_t           height,
    enum bj_pixel_mode    mode,
    size_t           stride
) {
    const size_t computed_stride = bj_compute_bitmap_stride(width, mode);
    if(stride < computed_stride) {
        stride = computed_stride;
    }

    if(bitmap) {
        bj_memset(bitmap, 0, sizeof(struct bj_bitmap));

        if(stride > 0) {
            size_t bufsize = stride * height;

            bitmap->width       = width;
            bitmap->height      = height;
            bitmap->stride      = stride;
            bitmap->mode        = mode;
            bitmap->clear_color = 0x00000000;
            bitmap->weak        = (pixels != 0);

            if (bitmap->weak) {
                bitmap->buffer = pixels;
            } else {
                bitmap->buffer = bj_malloc(bufsize);
                if (bitmap->buffer == 0) {
                    return 0;
                }
                bj_memset(bitmap->buffer, 0x00, bufsize);
            }

        }
    }
    return bitmap;
}

void bj_reset_bitmap(
    struct bj_bitmap* bitmap
) {
    bj_check(bitmap);

    if(bitmap->weak == 0) {
        bj_free(bitmap->buffer);
    }
    bitmap->buffer = 0;
    bj_destroy_bitmap(bitmap->charset);
}

struct bj_bitmap* bj_create_bitmap(
    size_t           width,
    size_t           height,
    enum bj_pixel_mode    mode,
    size_t           stride
) {
    struct bj_bitmap temp_bitmap;
    if(bj_init_bitmap(&temp_bitmap, 0, width, height, mode, stride) == 0) {
        return 0;
    }
    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        bj_free(temp_bitmap.buffer);
        return 0;
    }
    return bj_memcpy(new, &temp_bitmap, sizeof(struct bj_bitmap));
}

struct bj_bitmap* bj_create_bitmap_from_pixels(
    void*            pixels,
    size_t           width,
    size_t           height,
    enum bj_pixel_mode    mode,
    size_t           stride
) {
    bj_check_or_0(pixels);
    struct bj_bitmap temp_bitmap;
    if (bj_init_bitmap(&temp_bitmap, pixels, width, height, mode, stride) == 0) {
        return 0;
    }
    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        return 0;
    }
    return bj_memcpy(new, &temp_bitmap, sizeof(struct bj_bitmap));
}

struct bj_bitmap* bj_copy_bitmap(
    const struct bj_bitmap* bitmap
) {
    bj_check_or_0(bitmap);
    struct bj_bitmap temp_bitmap;
    if (bj_init_bitmap(&temp_bitmap, 0, bitmap->width, bitmap->height, bitmap->mode, bitmap->stride) == 0) {
        return 0;
    }
    bj_memcpy(temp_bitmap.buffer, bitmap->buffer, temp_bitmap.stride * temp_bitmap.height);
    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        bj_free(temp_bitmap.buffer);
        return 0;
    }
    return bj_memcpy(new, &temp_bitmap, sizeof(struct bj_bitmap));
}

struct bj_bitmap* bj_convert_bitmap(
    const struct bj_bitmap* src,
    enum bj_pixel_mode    mode
) {
    bj_check_or_0(src);
    bj_check_or_0(mode);

    if (src->mode == mode) {
        return bj_copy_bitmap(src);
    }
    struct bj_bitmap dst;
    if (bj_init_bitmap(&dst, 0, src->width, src->height, src->mode, src->stride) == 0) {
        return 0;
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    for (size_t y = 0; y < dst.height; ++y) {
        for (size_t x = 0; x < dst.width; ++x) {
            bj_make_bitmap_rgb(src, x, y, &r, &g, &b);
            bj_put_pixel(&dst, x, y, bj_make_bitmap_pixel(&dst, r, g, b));
        }
    }

    struct bj_bitmap* new = bj_allocate_bitmap();
    if (new == 0) {
        bj_free(dst.buffer);
        return 0;
    }
    return bj_memcpy(new, &dst, sizeof(struct bj_bitmap));
}

void bj_destroy_bitmap(
    struct bj_bitmap*     bitmap
) {
    bj_reset_bitmap(bitmap);
    bj_free(bitmap);
}

size_t bj_bitmap_width(
    const struct bj_bitmap*     bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->width;
}

size_t bj_bitmap_height(
    const struct bj_bitmap*     bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->height;
}

int bj_bitmap_mode(
    struct bj_bitmap* bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->mode;
}

size_t bj_bitmap_stride(
    struct bj_bitmap* bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->stride;
}


void* bj_bitmap_pixels(
    struct bj_bitmap*     bitmap
) {
    bj_check_or_0(bitmap);
    return bitmap->buffer;
}

uint32_t bj_make_bitmap_pixel(
    struct bj_bitmap* bitmap,
    uint8_t red,
    uint8_t green,
    uint8_t blue
) {
    bj_check_or_0(bitmap);
    return bj_get_pixel_value(bitmap->mode, red, green, blue);
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
    struct bj_bitmap* bitmap,
    size_t x,
    size_t y,
    uint32_t pixel
) {
    bj_check(bitmap);
    bj_check(x < bitmap->width && y < bitmap->height);
    buffer_set_pixel(x, y, bitmap->stride, bitmap->buffer, pixel, BJ_PIXEL_GET_BPP(bitmap->mode));
}

uint32_t bj_bitmap_pixel(
    const struct bj_bitmap* bitmap,
    size_t           x,
    size_t           y
) {
    bj_check_or_0(bitmap);
    bj_check_or_0(x < bitmap->width && y < bitmap->height);
    return buffer_get_pixel(x, y, bitmap->stride, bitmap->buffer, BJ_PIXEL_GET_BPP(bitmap->mode));
}

struct bj_bitmap* bj_create_bitmap_from_file(
    const char*       path,
    struct bj_error**        error
) {
    struct bj_error* inner_error = 0;

    struct bj_stream* stream = bj_open_stream_file(path, &inner_error);
    if(inner_error) {
        bj_forward_error(inner_error, error);
        return 0;
    }

    struct bj_bitmap* bitmap = dib_create_bitmap_from_stream(stream, error);
    bj_close_stream(stream);
    return bitmap;
}

void bj_clear_bitmap(struct bj_bitmap* bitmap) {
    bj_check(bitmap);

    bj_draw_line(bitmap, 0, 0, bitmap->width - 1, 0, bitmap->clear_color);
    if (bitmap->height > 1) {
        void* first_row = bitmap->buffer;
        for (size_t y = 1; y < bitmap->height; ++y) {
            void* dest_row = (uint8_t*)bitmap->buffer + (bitmap->stride * y);
            bj_memcpy(dest_row, first_row, bitmap->stride);
        }
    }
}


void bj_set_bitmap_clear_color(
    struct bj_bitmap* bitmap,
    uint32_t clear_color
) {
    bj_check(bitmap);
    bitmap->clear_color = clear_color;
}



void bj_make_bitmap_rgb(
    const struct bj_bitmap* bitmap,
    size_t           x,
    size_t           y,
    uint8_t*         red,
    uint8_t*         green,
    uint8_t*         blue
) {
    bj_check(bitmap);
    bj_make_pixel_rgb(
        bitmap->mode,
        bj_bitmap_pixel(bitmap, x, y),
        red,
        green,
        blue
    );
}


