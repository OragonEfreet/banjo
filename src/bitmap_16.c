// bitmap_16.c - 16bpp (RGB565, XRGB1555) operations.
//
// This file contains all bitmap operations for 16-bit pixels.
// Supports both RGB565 and XRGB1555 formats.
//
// Note: blit_mask operations currently fall back to generic implementation.
// They can be optimized here in the future if needed.

#include <string.h>

#include "bitmap.h"

// ----------------------------------------------------------------------------
// Mask Blit (non-stretched) - falls back to generic
// ----------------------------------------------------------------------------

void bj_blit_mask_16(
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

void bj_blit_mask_stretched_16(
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

void bj_fill_rect_16(
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
    const uint16_t p16 = (uint16_t)pixel;

    // Fill first row
    uint8_t* first_row_base = bj_row_ptr(dst, (size_t)y0);
    uint16_t* first_row = (uint16_t*)first_row_base + x0;
    for (size_t x = 0; x < width; ++x) {
        first_row[x] = p16;
    }

    // Copy first row to remaining rows (memcpy is heavily optimized)
    const size_t row_bytes = width * sizeof(uint16_t);
    for (int y = y0 + 1; y < y1; ++y) {
        uint8_t* dest_row = bj_row_ptr(dst, (size_t)y);
        memcpy((uint16_t*)dest_row + x0, first_row, row_bytes);
    }
}

// ----------------------------------------------------------------------------
// Horizontal Line - for circles/triangles
// ----------------------------------------------------------------------------

void bj_hline_16(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel) {
    // Clip
    if (y < 0 || y >= (int)dst->height) return;
    if (x0 < 0) x0 = 0;
    if (x1 > (int)dst->width) x1 = (int)dst->width;
    if (x0 >= x1) return;

    uint16_t* row = (uint16_t*)bj_row_ptr(dst, (size_t)y) + x0;
    const uint16_t p16 = (uint16_t)pixel;
    const size_t count = (size_t)(x1 - x0);

    for (size_t i = 0; i < count; ++i) {
        row[i] = p16;
    }
}
