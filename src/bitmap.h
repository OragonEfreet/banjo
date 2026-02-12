#pragma once

#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>

struct bj_bitmap {
    size_t             width;
    size_t             height;
    size_t             stride;
    enum bj_pixel_mode mode;
    uint32_t           clear_color;
    void*              buffer;
    int                weak;
    bj_bool            colorkey_enabled;
    uint32_t           colorkey;
    struct bj_bitmap*  charset;
};

// ============================================================================
// FAST PIXEL ACCESSORS - For internal use in hot loops only!
// ============================================================================
// These functions perform NO validation (no null checks, no bounds checks).
// The caller MUST ensure:
//   1. The bitmap pointer is valid
//   2. Coordinates are within bounds
//   3. The correct function is used for the bitmap's pixel mode
//
// Use these in inner loops where:
//   - The bitmap has already been validated at function entry
//   - Loop bounds are derived from bitmap dimensions (guaranteeing validity)
//   - The pixel mode is known/checked once before the loop
//
// These eliminate ~15-25 cycles of overhead per pixel vs bj_put_pixel().
// ============================================================================

// --------------------------------------------------------------------------
// Row pointer helpers - compute once per scanline, reuse for all pixels
// --------------------------------------------------------------------------

// Get pointer to the start of row `y` in the bitmap buffer.
static inline uint8_t* bj_row_ptr(const struct bj_bitmap* bmp, size_t y) {
    return (uint8_t*)bmp->buffer + y * bmp->stride;
}

// --------------------------------------------------------------------------
// 32-bit pixel access (XRGB8888) - most common format
// --------------------------------------------------------------------------

// Write a 32-bit pixel directly. No checks.
static inline void bj_put_pixel_32(uint8_t* row, size_t x, uint32_t pixel) {
    ((uint32_t*)row)[x] = pixel;
}

// Read a 32-bit pixel directly. No checks.
static inline uint32_t bj_get_pixel_32(const uint8_t* row, size_t x) {
    return ((const uint32_t*)row)[x];
}

// --------------------------------------------------------------------------
// 24-bit pixel access (BGR24) - byte-packed, no padding
// --------------------------------------------------------------------------

// Write a 24-bit BGR pixel. Pixel format: 0x00RRGGBB in memory as BB GG RR.
static inline void bj_put_pixel_24(uint8_t* row, size_t x, uint32_t pixel) {
    uint8_t* p = row + x * 3;
    p[0] = (uint8_t)(pixel);        // Blue
    p[1] = (uint8_t)(pixel >> 8);   // Green
    p[2] = (uint8_t)(pixel >> 16);  // Red
}

// Read a 24-bit BGR pixel, returned as 0x00RRGGBB.
static inline uint32_t bj_get_pixel_24(const uint8_t* row, size_t x) {
    const uint8_t* p = row + x * 3;
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
}

// --------------------------------------------------------------------------
// 16-bit pixel access (RGB565, XRGB1555)
// --------------------------------------------------------------------------

// Write a 16-bit pixel directly. No checks.
static inline void bj_put_pixel_16(uint8_t* row, size_t x, uint16_t pixel) {
    ((uint16_t*)row)[x] = pixel;
}

// Read a 16-bit pixel directly. No checks.
static inline uint16_t bj_get_pixel_16(const uint8_t* row, size_t x) {
    return ((const uint16_t*)row)[x];
}

// --------------------------------------------------------------------------
// 8-bit pixel access (Indexed, grayscale masks)
// --------------------------------------------------------------------------

// Write an 8-bit pixel directly. No checks.
static inline void bj_put_pixel_8(uint8_t* row, size_t x, uint8_t pixel) {
    row[x] = pixel;
}

// Read an 8-bit pixel directly. No checks.
static inline uint8_t bj_get_pixel_8(const uint8_t* row, size_t x) {
    return row[x];
}

// --------------------------------------------------------------------------
// Generic pixel access by BPP - slightly slower but flexible
// Use when pixel mode varies or is checked once before a loop.
// --------------------------------------------------------------------------

// Write a pixel given pre-computed BPP. For 8/16/24/32 bpp only.
static inline void bj_put_pixel_by_bpp(uint8_t* row, size_t x, uint32_t pixel, size_t bpp) {
    switch (bpp) {
        case 32: bj_put_pixel_32(row, x, pixel); break;
        case 24: bj_put_pixel_24(row, x, pixel); break;
        case 16: bj_put_pixel_16(row, x, (uint16_t)pixel); break;
        case 8:  bj_put_pixel_8(row, x, (uint8_t)pixel); break;
        default: break; // Sub-byte formats not supported here
    }
}

// Read a pixel given pre-computed BPP. For 8/16/24/32 bpp only.
static inline uint32_t bj_get_pixel_by_bpp(const uint8_t* row, size_t x, size_t bpp) {
    switch (bpp) {
        case 32: return bj_get_pixel_32(row, x);
        case 24: return bj_get_pixel_24(row, x);
        case 16: return bj_get_pixel_16(row, x);
        case 8:  return bj_get_pixel_8(row, x);
        default: return 0; // Sub-byte formats not supported here
    }
}

// ============================================================================
// FORMAT-SPECIFIC DISPATCH FUNCTIONS
// ============================================================================
// These functions are implemented in format-specific files (bitmap_32.c, etc.)
// or in bitmap.c (generic fallbacks). The dispatch happens once per operation,
// not per pixel, eliminating per-pixel function call overhead.

// ============================================================================
// Mask Blit Operations
// ============================================================================
// These functions perform masked blitting for text/glyph rendering.
// The mask is always 8bpp (coverage values 0-255).
// FG/BG colors are pre-unpacked to RGB components by the caller.

// 32bpp (XRGB8888) mask blit - most common, highly optimized
void bj_blit_mask_32(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,  // FG as RGB
    uint8_t br, uint8_t bg, uint8_t bb,  // BG as RGB
    bj_mask_bg_mode         mode
);

void bj_blit_mask_stretched_32(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

// 24bpp (BGR24) mask blit
void bj_blit_mask_24(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

void bj_blit_mask_stretched_24(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

// 16bpp (RGB565, XRGB1555) mask blit
void bj_blit_mask_16(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

void bj_blit_mask_stretched_16(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

// Generic fallback - uses bj_put_pixel/bj_bitmap_pixel, works for any format
void bj_blit_mask_generic(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

void bj_blit_mask_stretched_generic(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area,
    uint32_t                fg_native,
    uint32_t                bg_native,
    uint8_t fr, uint8_t fg, uint8_t fb,
    uint8_t br, uint8_t bg, uint8_t bb,
    bj_mask_bg_mode         mode
);

// ============================================================================
// Filled Rectangle Operations
// ============================================================================
// Fast rectangle fill for each format. Used by bj_draw_filled_rectangle
// and bj_clear_bitmap.

void bj_fill_rect_32(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
);

void bj_fill_rect_24(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
);

void bj_fill_rect_16(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
);

void bj_fill_rect_generic(
    struct bj_bitmap* dst,
    int x0, int y0,
    int x1, int y1,
    uint32_t pixel
);

// ============================================================================
// Horizontal Line Operations (for circles, triangles, etc.)
// ============================================================================
// Fast horizontal span fill. Coordinates are NOT clipped - caller must ensure
// y is in bounds and x0 <= x1.

void bj_hline_32(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel);
void bj_hline_24(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel);
void bj_hline_16(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel);
void bj_hline_generic(struct bj_bitmap* dst, int x0, int x1, int y, uint32_t pixel);



