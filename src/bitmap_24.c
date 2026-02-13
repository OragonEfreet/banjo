// bitmap_24.c - 24bpp (BGR24) operations.
//
// This file contains all bitmap operations for 24-bit pixels.
// Pixel format: BGR (3 bytes per pixel, no padding)
//
// Note: blit_mask operations currently fall back to generic implementation.
// They can be optimized here in the future if needed.

#include <string.h>

#include "bitmap.h"

// ----------------------------------------------------------------------------
// Mask Blit (non-stretched) - falls back to generic
// ----------------------------------------------------------------------------

void bj_blit_mask_24(
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
    bj_blit_mask_generic(mask, ms, dst, ds, fg_native, bg_native,
                          fr, fg, fb, br, bg, bb, mode);
}

// ----------------------------------------------------------------------------
// Mask Blit (stretched) - falls back to generic
// ----------------------------------------------------------------------------

void bj_blit_mask_stretched_24(
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
    bj_blit_mask_stretched_generic(mask, ms, dst, ds, fg_native, bg_native,
                                    fr, fg, fb, br, bg, bb, mode);
}

// ----------------------------------------------------------------------------
// Filled Rectangle
// ----------------------------------------------------------------------------

void bj_fill_rect_24(
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

    // Unpack pixel to BGR bytes
    const uint8_t b = (uint8_t)(pixel);
    const uint8_t g = (uint8_t)(pixel >> 8);
    const uint8_t r = (uint8_t)(pixel >> 16);

    // Fill first row
    uint8_t* first_row = bj_row_ptr(dst, (size_t)y0) + (size_t)x0 * 3;
    uint8_t* p = first_row;
    for (size_t x = 0; x < width; ++x) {
        *p++ = b;
        *p++ = g;
        *p++ = r;
    }

    // Copy first row to remaining rows (memcpy is heavily optimized)
    const size_t row_bytes = width * 3;
    for (int y = y0 + 1; y < y1; ++y) {
        uint8_t* dest_row = bj_row_ptr(dst, (size_t)y) + (size_t)x0 * 3;
        memcpy(dest_row, first_row, row_bytes);
    }
}

// ----------------------------------------------------------------------------
// Horizontal Line - for circles/triangles
// ----------------------------------------------------------------------------

void bj_hline_24(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel) {
    // Clip
    if (y < 0 || y >= (int)dst->height) return;
    if (x0 < 0) x0 = 0;
    if (x1 > (int)dst->width) x1 = (int)dst->width;
    if (x0 >= x1) return;

    uint8_t* row = bj_row_ptr(dst, (size_t)y) + x0 * 3;
    const uint8_t b = (uint8_t)(pixel);
    const uint8_t g = (uint8_t)(pixel >> 8);
    const uint8_t r = (uint8_t)(pixel >> 16);

    for (int x = x0; x < x1; ++x) {
        *row++ = b;
        *row++ = g;
        *row++ = r;
    }
}
