// bitmap_32.c - 32bpp (XRGB8888) optimized operations.
//
// This file contains all bitmap operations optimized for 32-bit pixels.
// Pixel format: 0x00RRGGBB (8 bits per channel, high byte unused)
//
// Optimizations applied:
//   - Direct 32-bit memory access (no byte unpacking)
//   - Row pointers computed once per scanline
//   - Division-free alpha blending (see bj_mix_u8)
//   - No function call overhead in inner loops

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "bitmap.h"

// ----------------------------------------------------------------------------
// Pixel Accessors - static inline, file-local
// ----------------------------------------------------------------------------

// Get pointer to start of row y.
static inline uint8_t* row_ptr(const struct bj_bitmap* bmp, size_t y) {
    return (uint8_t*)bmp->buffer + y * bmp->stride;
}

// Write 32-bit pixel at (row, x).
static inline void put_pixel(uint8_t* row, size_t x, uint32_t pixel) {
    ((uint32_t*)row)[x] = pixel;
}

// Read 32-bit pixel at (row, x).
static inline uint32_t get_pixel(const uint8_t* row, size_t x) {
    return ((const uint32_t*)row)[x];
}

// Pack RGB components to XRGB8888.
static inline uint32_t pack_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Unpack XRGB8888 to RGB components.
static inline void unpack_rgb(uint32_t pixel, uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = (pixel >> 16) & 0xFF;
    *g = (pixel >> 8) & 0xFF;
    *b = pixel & 0xFF;
}

// ----------------------------------------------------------------------------
// Alpha Blending - division-free
// ----------------------------------------------------------------------------

// Integer alpha blend: (d*(255-a) + s*a) / 255
// Uses: x/255 = (x + 1 + (x >> 8)) >> 8
// We combine rounding (+127) with +1 term -> +128
static inline uint8_t mix_u8(uint16_t alpha, uint8_t src, uint8_t dst) {
    uint32_t x = (uint32_t)dst * (255u - alpha) + (uint32_t)src * alpha;
    x += 128u + (x >> 8);
    return (uint8_t)(x >> 8);
}

// Blend source RGB over dest RGB with alpha.
static inline uint32_t blend_over(
    uint8_t alpha,
    uint8_t src_r, uint8_t src_g, uint8_t src_b,
    uint8_t dst_r, uint8_t dst_g, uint8_t dst_b
) {
    return pack_rgb(
        mix_u8(alpha, src_r, dst_r),
        mix_u8(alpha, src_g, dst_g),
        mix_u8(alpha, src_b, dst_b)
    );
}

// ----------------------------------------------------------------------------
// Fixed-point constants for stretched blits
// ----------------------------------------------------------------------------

// We use 16.16 fixed-point for coordinate interpolation.
// This avoids per-pixel division in the inner loop.
#define FRAC_BITS 16

// ----------------------------------------------------------------------------
// Mask Blit (non-stretched)
// ----------------------------------------------------------------------------

void bj_blit_mask_32(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    for (uint16_t row = 0; row < ds->h; ++row) {
        const size_t my = (size_t)ms->y + row;
        const size_t dy = (size_t)ds->y + row;

        const uint8_t* mrow = row_ptr(mask, my);
        uint8_t*       drow = row_ptr(dst, dy);

        for (uint16_t col = 0; col < ds->w; ++col) {
            const size_t mx = (size_t)ms->x + col;
            const size_t dx = (size_t)ds->x + col;
            const uint8_t alpha = mrow[mx];

            switch (mode) {
            case BJ_MASK_BG_TRANSPARENT:
                if (alpha == 0) continue;
                if (alpha == 255) {
                    put_pixel(drow, dx, fg_native);
                } else {
                    uint32_t dval = get_pixel(drow, dx);
                    uint8_t dst_r, dst_g, dst_b;
                    unpack_rgb(dval, &dst_r, &dst_g, &dst_b);
                    put_pixel(drow, dx, blend_over(alpha, fr, fg, fb, dst_r, dst_g, dst_b));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                if (alpha == 0) {
                    put_pixel(drow, dx, bg_native);
                } else if (alpha == 255) {
                    put_pixel(drow, dx, fg_native);
                } else {
                    put_pixel(drow, dx, blend_over(alpha, fr, fg, fb, br, bg, bb));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                const uint8_t alpha_bg = (uint8_t)(255u - alpha);
                if (alpha_bg == 0) {
                    // Inside glyph: keep dst
                } else if (alpha_bg == 255) {
                    put_pixel(drow, dx, bg_native);
                } else {
                    uint32_t dval = get_pixel(drow, dx);
                    uint8_t dst_r, dst_g, dst_b;
                    unpack_rgb(dval, &dst_r, &dst_g, &dst_b);
                    put_pixel(drow, dx, blend_over(alpha_bg, br, bg, bb, dst_r, dst_g, dst_b));
                }
            } break;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Mask Blit (stretched, nearest-neighbor)
// ----------------------------------------------------------------------------

void bj_blit_mask_stretched_32(
    const struct bj_bitmap* mask,
    const struct bj_rect*   ms,
    struct bj_bitmap*       dst,
    const struct bj_rect*   ds,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
) {
    const size_t sw = ms->w, sh = ms->h;
    const size_t dw = ds->w, dh = ds->h;

    if (dw == 0 || dh == 0) return;

    // Fixed-point step values: computed ONCE before the loops
    const uint32_t y_step = ((uint32_t)sh << FRAC_BITS) / (uint32_t)dh;
    const uint32_t x_step = ((uint32_t)sw << FRAC_BITS) / (uint32_t)dw;

    uint32_t y_accum = 0;

    for (uint16_t dy = 0; dy < dh; ++dy) {
        const size_t sy = (size_t)ms->y + (y_accum >> FRAC_BITS);
        const size_t out_y = (size_t)ds->y + dy;
        y_accum += y_step;

        const uint8_t* mrow = row_ptr(mask, sy);
        uint8_t*       drow = row_ptr(dst, out_y);

        uint32_t x_accum = 0;

        for (uint16_t dx = 0; dx < dw; ++dx) {
            const size_t sx = (size_t)ms->x + (x_accum >> FRAC_BITS);
            const size_t out_x = (size_t)ds->x + dx;
            x_accum += x_step;

            const uint8_t alpha = mrow[sx];

            switch (mode) {
            case BJ_MASK_BG_TRANSPARENT:
                if (alpha == 0) continue;
                if (alpha == 255) {
                    put_pixel(drow, out_x, fg_native);
                } else {
                    uint32_t dval = get_pixel(drow, out_x);
                    uint8_t dst_r, dst_g, dst_b;
                    unpack_rgb(dval, &dst_r, &dst_g, &dst_b);
                    put_pixel(drow, out_x, blend_over(alpha, fr, fg, fb, dst_r, dst_g, dst_b));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                if (alpha == 0) {
                    put_pixel(drow, out_x, bg_native);
                } else if (alpha == 255) {
                    put_pixel(drow, out_x, fg_native);
                } else {
                    put_pixel(drow, out_x, blend_over(alpha, fr, fg, fb, br, bg, bb));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                const uint8_t alpha_bg = (uint8_t)(255u - alpha);
                if (alpha_bg == 0) { } // keep dst
                else if (alpha_bg == 255) {
                    put_pixel(drow, out_x, bg_native);
                } else {
                    uint32_t dval = get_pixel(drow, out_x);
                    uint8_t dst_r, dst_g, dst_b;
                    unpack_rgb(dval, &dst_r, &dst_g, &dst_b);
                    put_pixel(drow, out_x, blend_over(alpha_bg, br, bg, bb, dst_r, dst_g, dst_b));
                }
            } break;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Filled Rectangle
// ----------------------------------------------------------------------------

void bj_fill_rect_32(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
) {
    // Clip to bitmap bounds
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (int)dst->width)  x1 = (int)dst->width;
    if (y1 > (int)dst->height) y1 = (int)dst->height;
    if (x0 >= x1 || y0 >= y1) return;

    const size_t width = (size_t)(x1 - x0);

    // Fill first row
    uint8_t* first_row = row_ptr(dst, (size_t)y0);
    uint32_t* p = (uint32_t*)first_row + x0;
    for (size_t x = 0; x < width; ++x) {
        p[x] = pixel;
    }

    // Copy first row to remaining rows (memcpy is heavily optimized)
    const size_t row_bytes = width * sizeof(uint32_t);
    for (int y = y0 + 1; y < y1; ++y) {
        uint8_t* dest_row = row_ptr(dst, (size_t)y);
        memcpy(dest_row + x0 * sizeof(uint32_t), first_row + x0 * sizeof(uint32_t), row_bytes);
    }
}

// ----------------------------------------------------------------------------
// Horizontal Line - for circles/triangles
// ----------------------------------------------------------------------------

void bj_hline_32(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel) {
    // Clip
    if (y < 0 || y >= (int)dst->height) return;
    if (x0 < 0) x0 = 0;
    if (x1 > (int)dst->width) x1 = (int)dst->width;
    if (x0 >= x1) return;

    uint32_t* row = (uint32_t*)row_ptr(dst, (size_t)y) + x0;
    const size_t count = (size_t)(x1 - x0);
    for (size_t i = 0; i < count; ++i) {
        row[i] = pixel;
    }
}
