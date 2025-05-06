#include <banjo/log.h>
#include <banjo/pixel.h>
#include <banjo/stream.h>

#include "bitmap_t.h"
#include "check.h"

#include <math.h>

#define ABS_INT(x) ((x) < 0 ? -(x) : (x))

bj_bitmap* dib_create_bitmap_from_stream(bj_stream*, bj_error**);

BANJO_EXPORT bj_bitmap* bj_bitmap_alloc(
    void
) {
    return bj_malloc(sizeof(bj_bitmap));
}

bj_bitmap* bj_bitmap_init(
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

            p_bitmap->width = width;
            p_bitmap->height = height;
            p_bitmap->stride = stride;
            p_bitmap->mode = mode;
            p_bitmap->clear_color = 0x00000000;
            p_bitmap->weak = (p_pixels != 0);

            if (p_bitmap->weak) {
                p_bitmap->buffer = p_pixels;
            } else {
                p_bitmap->buffer = bj_malloc(bufsize);
                bj_memset(p_bitmap->buffer, 0x00, bufsize);
            }
            
        }
    }
    return p_bitmap;
}

void bj_bitmap_reset(
    bj_bitmap* p_bitmap
) {
    bj_check(p_bitmap);

    if(p_bitmap->weak == 0) {
        bj_free(p_bitmap->buffer);
    }
}

bj_bitmap* bj_bitmap_new(
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
) {
    bj_bitmap bitmap;
    if(bj_bitmap_init(&bitmap, 0, width, height, mode, stride) == 0) {
        return 0;
    }
    return bj_memcpy(bj_bitmap_alloc(), &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_bitmap_new_from_pixels(
    void*            p_pixels,
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
) {
    bj_bitmap bitmap;
    if (bj_bitmap_init(&bitmap, p_pixels, width, height, mode, stride) == 0) {
        return 0;
    }
    return bj_memcpy(bj_bitmap_alloc(), &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_bitmap_copy(
    const bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);
    bj_bitmap bitmap;
    if (bj_bitmap_init(&bitmap, 0, p_bitmap->width, p_bitmap->height, p_bitmap->mode, p_bitmap->stride) == 0) {
        return 0;
    }
    bj_memcpy(bitmap.buffer, p_bitmap->buffer, bitmap.stride * bitmap.height);
    return bj_memcpy(bj_bitmap_alloc(), &bitmap, sizeof(bj_bitmap));
}

bj_bitmap* bj_bitmap_convert(
    const bj_bitmap* p_src,
    bj_pixel_mode    mode
) {
    bj_check_or_0(p_src);
    bj_check_or_0(mode);

    if (p_src->mode == mode) {
        return bj_bitmap_copy(p_src);
    }
    bj_bitmap dst;
    if (bj_bitmap_init(&dst, 0, p_src->width, p_src->height, p_src->mode, p_src->stride) == 0) {
        return 0;
    }
    
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    for (size_t y = 0; y < dst.height; ++y) {
        for (size_t x = 0; x < dst.width; ++x) {
            bj_bitmap_rgb(p_src, x, y, &r, &g, &b);
            bj_bitmap_put_pixel(&dst, x, y, bj_bitmap_pixel_value(&dst, r, g, b));
        }
    }

    return bj_memcpy(bj_bitmap_alloc(), &dst, sizeof(bj_bitmap));
}

void bj_bitmap_del(
    bj_bitmap*     p_bitmap
) {
    bj_bitmap_reset(p_bitmap);
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
    //pixel_value &= (1u << bpp) - 1; // TODO This does not work
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
    int x0 = p0[X];
    int y0 = p0[Y];
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

static int bj_bitmap_clipped_blit(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    bj_rect* p_dest_rect
) {
    // This is the "stupid blit". Just to test clipping works
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    
    bj_check_or_0(p_src);
    bj_check_or_0(p_dest);
    bj_check_or_0(p_src_rect->w == p_dest_rect->w);
    bj_check_or_0(p_src_rect->h == p_dest_rect->h);

    for(size_t r = 0 ; r < p_dest_rect->h ; ++r) {

        const size_t from_y = p_src_rect->y + r;
        const size_t to_y = p_dest_rect->y + r;
        for(size_t c = 0 ; c < p_dest_rect->w ; ++c) {
            bj_bitmap_rgb(p_src, p_src_rect->x + c, from_y, &red, &green, &blue);
            bj_bitmap_put_pixel(p_dest,
                p_dest_rect->x + c,
                to_y,
                bj_bitmap_pixel_value(p_dest, red, green, blue)
            );
        }
    }
    return 1;
}

int bj_bitmap_clipped_blit_stretched(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    const bj_rect* p_dest_rect
) {
    // This is the "stupid blit". Just to test clipping works
    bj_check_or_0(p_src);
    bj_check_or_0(p_dest);
    bj_check_or_0(p_src_rect);
    bj_check_or_0(p_dest_rect);
    bj_check_or_0(p_src_rect->w > 0 && p_src_rect->h > 0);
    bj_check_or_0(p_dest_rect->w > 0 && p_dest_rect->h > 0);

    const double scale_x = (double)p_src_rect->w / p_dest_rect->w;
    const double scale_y = (double)p_src_rect->h / p_dest_rect->h;

    uint8_t red = 0, green = 0, blue = 0;

    for (int dy = 0; dy < p_dest_rect->h; ++dy) {
        int sy = (int)(p_src_rect->y + dy * scale_y);
        if (sy < 0 || sy >= (int)p_src->height) continue;

        for (int dx = 0; dx < p_dest_rect->w; ++dx) {
            int sx = (int)(p_src_rect->x + dx * scale_x);
            if (sx < 0 || sx >= (int)p_src->width) continue;

            bj_bitmap_rgb(p_src, sx, sy, &red, &green, &blue);
            bj_bitmap_put_pixel(
                p_dest,
                p_dest_rect->x + dx,
                p_dest_rect->y + dy,
                bj_bitmap_pixel_value(p_dest, red, green, blue)
            );
        }
    }

    return 1;
}

bool bj_bitmap_blit(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    const bj_rect* p_dest_rect
) {

    
    bj_check_or_0(p_src);
    bj_check_or_0(p_dest);

    bj_rect src_rect = {.x = 0, .y = 0, .w = p_src->width, .h = p_src->height};
    bj_rect dest_rect = { 0 };

    if (p_dest_rect) {
        dest_rect.x = p_dest_rect->x;
        dest_rect.y = p_dest_rect->y;
    }

    // Clip source area
    if (p_src_rect) {
        bj_rect intersection = { 0 };
        if (bj_rect_intersect(p_src_rect, &src_rect, &intersection) == 0) {
            return 0;
        }
        dest_rect.x += intersection.x - p_src_rect->x;
        dest_rect.y += intersection.y - p_src_rect->y;
        src_rect = intersection;
    }

    dest_rect.w = src_rect.w;
    dest_rect.h = src_rect.h;

    bj_rect intersection = { 0 };
    if (bj_rect_intersect(&dest_rect, &(bj_rect){.x = 0, .y = 0, .w = p_dest->width, .h = p_dest->height}, & intersection) == 0) {
        return 0;
    }

    // Adjust src rect
    src_rect.x += intersection.x - dest_rect.x;
    src_rect.y += intersection.y - dest_rect.y;
    src_rect.w = intersection.w;
    src_rect.h = intersection.h;
    dest_rect = intersection;

    if (dest_rect.w <= 0 || dest_rect.h <= 0) {
        return 0;
    }

    return bj_bitmap_clipped_blit(p_src, &src_rect, p_dest, &dest_rect);

}

bool bj_bitmap_blit_stretched(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    const bj_rect* p_dest_rect
) {
    if (!p_src || !p_dest) return false;

    int src_w = p_src_rect ? p_src_rect->w : p_src->width;
    int src_h = p_src_rect ? p_src_rect->h : p_src->height;
    int dest_w = p_dest_rect ? p_dest_rect->w : p_dest->width;
    int dest_h = p_dest_rect ? p_dest_rect->h : p_dest->height;

    if (src_w <= 0 || src_h <= 0 || dest_w <= 0 || dest_h <= 0)
        return false;

    if (dest_w == src_w && dest_h == src_h) {
        return bj_bitmap_blit(p_src, p_src_rect, p_dest, p_dest_rect);
    }

    const double scale_w = (double)dest_w / src_w;
    const double scale_h = (double)dest_h / src_h;

    double src_x0 = p_src_rect ? p_src_rect->x : 0.0;
    double src_y0 = p_src_rect ? p_src_rect->y : 0.0;
    double src_x1 = src_x0 + src_w;
    double src_y1 = src_y0 + src_h;

    double dst_x0 = p_dest_rect ? p_dest_rect->x : 0.0;
    double dst_y0 = p_dest_rect ? p_dest_rect->y : 0.0;
    double dst_x1 = dst_x0 + dest_w;
    double dst_y1 = dst_y0 + dest_h;

    if (src_x0 < 0) {
        dst_x0 -= src_x0 * scale_w;
        src_x0 = 0;
    }
    if (src_y0 < 0) {
        dst_y0 -= src_y0 * scale_h;
        src_y0 = 0;
    }
    if (src_x1 > p_src->width) {
        dst_x1 -= (src_x1 - p_src->width) * scale_w;
        src_x1 = p_src->width;
    }
    if (src_y1 > p_src->height) {
        dst_y1 -= (src_y1 - p_src->height) * scale_h;
        src_y1 = p_src->height;
    }

    const bj_rect clip_rect = { 0, 0, p_dest->width, p_dest->height };

    double adj;

    adj = dst_x0 - clip_rect.x;
    if (adj < 0) {
        src_x0 -= adj / scale_w;
        dst_x0 = clip_rect.x;
    }

    adj = dst_y0 - clip_rect.y;
    if (adj < 0) {
        src_y0 -= adj / scale_h;
        dst_y0 = clip_rect.y;
    }

    adj = dst_x1 - (clip_rect.x + clip_rect.w);
    if (adj > 0) {
        src_x1 -= adj / scale_w;
        dst_x1 = clip_rect.x + clip_rect.w;
    }

    adj = dst_y1 - (clip_rect.y + clip_rect.h);
    if (adj > 0) {
        src_y1 -= adj / scale_h;
        dst_y1 = clip_rect.y + clip_rect.h;
    }

    bj_rect clipped_src = {
        .x = (int)round(src_x0),
        .y = (int)round(src_y0),
        .w = (int)round(src_x1 - src_x0),
        .h = (int)round(src_y1 - src_y0),
    };

    bj_rect clipped_dest = {
        .x = (int)round(dst_x0),
        .y = (int)round(dst_y0),
        .w = (int)round(dst_x1 - dst_x0),
        .h = (int)round(dst_y1 - dst_y0),
    };

    if (clipped_src.w <= 0 || clipped_src.h <= 0 ||
        clipped_dest.w <= 0 || clipped_dest.h <= 0) {
        return false;
    }

    bj_rect src_bounds = { 0, 0, p_src->width, p_src->height };
    if (bj_rect_intersect(&src_bounds, &clipped_src, &clipped_src) == 0) {
        return false;
    }

    if (bj_rect_intersect(&clip_rect, &clipped_dest, &clipped_dest) == 0) {
        return false;
    }

    return bj_bitmap_clipped_blit_stretched(p_src, &clipped_src, p_dest, &clipped_dest);
}


void bj_bitmap_rgb(
    const bj_bitmap* p_bitmap,
    size_t           x,
    size_t           y,
    uint8_t*         p_red,
    uint8_t*         p_green,
    uint8_t*         p_blue
) {
    bj_check(p_bitmap);
    bj_pixel_rgb(
        p_bitmap->mode,
        bj_bitmap_get(p_bitmap, x, y),
        p_red,
        p_green,
        p_blue
    );
}


