#include <banjo/log.h>
#include <banjo/stream.h>

#include "bitmap_t.h"
#include "pixel_t.h"

#include <stdio.h>

#define ABS_INT(x) ((x) < 0 ? -(x) : (x))

bj_bitmap* dib_create_bitmap_from_stream(bj_stream*, bj_error**);

static size_t bitmap_stride(
    size_t width,
    bj_pixel_mode mode
) {
    switch(mode) {
        case BJ_PIXEL_MODE_INDEXED_1:
            return ((width + 7) / 8 + 3) & ~3;
        case BJ_PIXEL_MODE_INDEXED_4:
            return ((width + 1) / 2 + 3) & ~3;
        case BJ_PIXEL_MODE_INDEXED_8:
            return (width + 3) & ~3;
        case BJ_PIXEL_MODE_RGB565:
        case BJ_PIXEL_MODE_XRGB1555:
            return (width * 2 + 3) & ~3;
        case BJ_PIXEL_MODE_BGR24:
            return (width * 3 + 3) & ~3;
        case BJ_PIXEL_MODE_XRGB8888:
            return width * 4;
        default: break;

    }
    return 0;
}

bj_bitmap* bj_bitmap_init(
    bj_bitmap*      p_bitmap,
    size_t           width,
    size_t           height,
    bj_pixel_mode   mode,
    size_t           stride
) {
    const size_t computed_stride = bitmap_stride(width, mode);
    if(stride < computed_stride) {
        stride = computed_stride;
    }

    if(p_bitmap) {
        bj_memset(p_bitmap, 0, sizeof(bj_bitmap));

        if(stride > 0) {
            size_t bufsize = stride * height;

            p_bitmap->width = width;
            p_bitmap->height = height;
            p_bitmap->stride = stride;
            p_bitmap->buffer = bj_malloc(bufsize);
            p_bitmap->mode = mode;
            p_bitmap->clear_color = 0x00000000;
            bj_memset(p_bitmap->buffer, 0x00, bufsize);
        }
    }
    return p_bitmap;
}

void bj_bitmap_reset(
    bj_bitmap* p_bitmap
) {
    bj_check(p_bitmap);

    if(p_bitmap->buffer != 0) {
        bj_free(p_bitmap->buffer);
    }
#ifdef BJ_FEAT_PEDANTIC
    bj_memset(p_bitmap, 0, sizeof(bj_oldbmp));
#endif
}

bj_bitmap* bj_bitmap_new(
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
) {
    bj_bitmap bitmap;
    if(bj_bitmap_init(&bitmap, width, height, mode, stride) == 0) {
        return 0;
    }
    return bj_memcpy(bj_malloc(sizeof(bj_bitmap)), &bitmap, sizeof(bj_bitmap));
}

void bj_bitmap_del(
    bj_bitmap*     p_bitmap
) {
    bj_bitmap_reset(p_bitmap);
    bj_free(p_bitmap);
}

size_t bj_bitmap_width(
    bj_bitmap*     p_bitmap
) {
    bj_check_or_0(p_bitmap);
    return p_bitmap->width;
}

size_t bj_bitmap_height(
    bj_bitmap*     p_bitmap
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

int bj_bitmap_stride( 
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

uint32_t bj_bitmap_pixel_value(
    bj_bitmap* p_bitmap,
    uint8_t red,
    uint8_t green,
    uint8_t blue
) {
    bj_check_or_0(p_bitmap);
    return bj_pixel_value(p_bitmap->mode, red, green, blue);
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
    pixel_value &= (1u << bpp) - 1;
    return pixel_value;
}

void bj_bitmap_put_pixel(
    bj_bitmap* p_bitmap,
    size_t x,
    size_t y,
    uint32_t pixel
) {
    bj_check(p_bitmap);
    bj_check(x < p_bitmap->width && y < p_bitmap->height);
    buffer_set_pixel(x, y, p_bitmap->stride, p_bitmap->buffer, pixel, BJ_PIXEL_GET_BPP(p_bitmap->mode));
}

uint32_t bj_bitmap_get(
    const bj_bitmap* p_bitmap,
    size_t           x,
    size_t           y
) {
    bj_check_or_0(p_bitmap);
    bj_check_or_0(x < p_bitmap->width && y < p_bitmap->height);
    return buffer_get_pixel(x, y, p_bitmap->stride, p_bitmap->buffer, BJ_PIXEL_GET_BPP(p_bitmap->mode));
}

bj_bitmap* bj_bitmap_new_from_file(
    const char*       p_path,
    bj_error**        p_error
) {
    bj_error* p_inner_error = 0;

    bj_stream* p_stream = bj_stream_new_read_from_file(p_path, &p_inner_error);
    if(p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
        return 0;
    }

    bj_bitmap* p_bitmap = dib_create_bitmap_from_stream(p_stream, p_error);
    bj_stream_del(p_stream);
    return p_bitmap;
}

void bj_bitmap_clear(bj_bitmap* p_bitmap) {
    bj_check(p_bitmap);

    bj_bitmap_draw_line(p_bitmap, (bj_pixel){0, 0}, (bj_pixel){p_bitmap->width - 1, 0}, p_bitmap->clear_color);
    if (p_bitmap->height > 1) {
        void* first_row = p_bitmap->buffer;
        for (size_t y = 1; y < p_bitmap->height; ++y) {
            void* dest_row = (uint8_t*)p_bitmap->buffer + (p_bitmap->stride * y);
            bj_memcpy(dest_row, first_row, p_bitmap->stride);
        }
    }
}

#define X 0
#define Y 1

BANJO_EXPORT void bj_bitmap_draw_line(
    bj_bitmap*     bmp,
    bj_pixel       p0,
    bj_pixel       p1,
    uint32_t       c
) {
    /// Bresenham's line algorithm
    int x0 = p0[X]; int y0 = p0[Y];
    const int x1 = p1[X]; const int y1 = p1[Y];

    const int dx = ABS_INT(x1 - x0);
    const int dy = ABS_INT(y1 - y0);
    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        bj_bitmap_put_pixel(bmp, x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        const int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void bj_bitmap_draw_triangle(
    bj_bitmap* bmp,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    uint32_t c
) {
    bj_bitmap_draw_line(bmp, p0, p1, c);
    bj_bitmap_draw_line(bmp, p1, p2, c);
    bj_bitmap_draw_line(bmp, p2, p0, c);
}

void bj_bitmap_set_clear_color(
    bj_bitmap* p_bitmap,
    uint32_t clear_color
) {
    bj_check(p_bitmap);
    p_bitmap->clear_color = clear_color;
}

bool bj_bitmap_blit(
    const bj_bitmap*  p_src,
    const bj_rect*    p_src_rect,
    bj_bitmap*        p_dest,
    bj_rect*          p_dest_rect
) {
    // Clip the source rect
    bj_rect blit_rect;
    if( bj_rect_intersect(
        &(bj_rect){.w = p_src->width, .h = p_src->width, },
        p_src_rect, &blit_rect) == true
    ) {
        p_dest_rect->w = blit_rect.w;
        p_dest_rect->h = blit_rect.h;

        if( bj_rect_intersect(
            &(bj_rect) {.w = p_dest->width, .h = p_dest->height, },
            p_dest_rect, p_dest_rect) == true
        ) {
            for(size_t r = 0 ; r < p_dest_rect->h ; ++r) {
                const size_t from_y = blit_rect.y + r;
                const size_t to_y = p_dest_rect->y + r;
                for(size_t c = 0 ; c < p_dest_rect->w ; ++c) {
                    bj_bitmap_put_pixel(p_dest,
                        p_dest_rect->x + c,
                        to_y,
                        bj_bitmap_get(p_src, blit_rect.x + c, from_y)
                    );
                }
            }
        }

        return true;
    }

    return false;
}
