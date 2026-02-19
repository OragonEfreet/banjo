// bitmap_blit_mask.c - Masked blitting API (text/glyph rendering).
//
// This file contains the public API for mask blitting. It handles:
//   - Parameter validation
//   - Rectangle setup and clipping
//   - Color unpacking (native -> RGB)
//   - Dispatch to format-specific implementations
//
// The actual pixel loops are in format-specific files:
//   - bitmap_32.c (32bpp XRGB8888 - optimized)
//   - bitmap.c    (generic fallback for other formats)

#include <stdint.h>
#include <stddef.h>

#include <banjo/api.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>

#include "bitmap.h"
#include "check.h"

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

static inline size_t min_size_t(size_t val_a, size_t val_b) {
    return (val_a < val_b) ? val_a : val_b;
}

// Map dst i in [0..dst_len) to src in [0..src_len) (nearest neighbor)
static inline size_t map_nn(size_t i, size_t src_len, size_t dst_len) {
    return (dst_len == 0) ? 0 : (size_t)((uint64_t)i * (uint64_t)src_len / (uint64_t)dst_len);
}

// Validate & prepare rectangles (mask must be 8 bpp)
static bj_bool setup_mask_rects(
    const struct bj_bitmap* mask, const struct bj_rect* mask_area_in,
    struct bj_bitmap* dst, const struct bj_rect* dst_area_in,
    struct bj_rect* mask_area, struct bj_rect* dst_area)
{
    bj_check_or_0(mask && dst);

    // Require one byte per pixel mask
    if (BJ_PIXEL_GET_BPP(mask->mode) != 8u) return BJ_FALSE;

    struct bj_rect full_mask = (struct bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)min_size_t(mask->width,  UINT16_MAX),
        .h = (uint16_t)min_size_t(mask->height, UINT16_MAX)
    };

    *mask_area = mask_area_in ? *mask_area_in : full_mask;

    struct bj_rect default_dst = (struct bj_rect){ .x = 0, .y = 0, .w = mask_area->w, .h = mask_area->h };
    *dst_area = dst_area_in ? *dst_area_in : default_dst;

    // Clip mask area to mask bounds
    struct bj_rect mask_bounds = full_mask;
    if (bj_rect_intersection(&mask_bounds, mask_area, mask_area) == 0) return BJ_FALSE;
    if (mask_area->w == 0 || mask_area->h == 0) return BJ_FALSE;

    return BJ_TRUE;
}

// ----------------------------------------------------------------------------
// Format Dispatch
// ----------------------------------------------------------------------------

// Dispatch to the appropriate format-specific implementation.
// BPP check is done ONCE here, not per-pixel.
static void dispatch_blit_mask(
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
    const size_t bpp = BJ_PIXEL_GET_BPP(dst->mode);

    switch (bpp) {
    case 32:
        bj_blit_mask_32(mask, ms, dst, ds, fg_native, bg_native,
                         fr, fg, fb, br, bg, bb, mode);
        break;
    case 24:
        bj_blit_mask_24(mask, ms, dst, ds, fg_native, bg_native,
                         fr, fg, fb, br, bg, bb, mode);
        break;
    case 16:
        bj_blit_mask_16(mask, ms, dst, ds, fg_native, bg_native,
                         fr, fg, fb, br, bg, bb, mode);
        break;
    default:
        bj_blit_mask_generic(mask, ms, dst, ds, fg_native, bg_native,
                              fr, fg, fb, br, bg, bb, mode);
        break;
    }
}

static void dispatch_blit_mask_stretched(
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
    const size_t bpp = BJ_PIXEL_GET_BPP(dst->mode);

    switch (bpp) {
    case 32:
        bj_blit_mask_stretched_32(mask, ms, dst, ds, fg_native, bg_native,
                                   fr, fg, fb, br, bg, bb, mode);
        break;
    case 24:
        bj_blit_mask_stretched_24(mask, ms, dst, ds, fg_native, bg_native,
                                   fr, fg, fb, br, bg, bb, mode);
        break;
    case 16:
        bj_blit_mask_stretched_16(mask, ms, dst, ds, fg_native, bg_native,
                                   fr, fg, fb, br, bg, bb, mode);
        break;
    default:
        bj_blit_mask_stretched_generic(mask, ms, dst, ds, fg_native, bg_native,
                                        fr, fg, fb, br, bg, bb, mode);
        break;
    }
}

// ----------------------------------------------------------------------------
// Public API: Non-stretched mask blit
// ----------------------------------------------------------------------------

bj_bool bj_blit_mask(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area_in,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area_in,
    uint32_t                fg_native,
    uint32_t                bg_native,
    bj_mask_bg_mode         mode
) {
    struct bj_rect ms, ds;
    if (!setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;

    // Non-stretched: sizes must match
    if (ds.w != ms.w || ds.h != ms.h) return BJ_FALSE;

    // Clip destination to bounds and adjust source accordingly
    struct bj_rect dst_bounds = (struct bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)min_size_t(dst->width,  UINT16_MAX),
        .h = (uint16_t)min_size_t(dst->height, UINT16_MAX)
    };
    struct bj_rect inter;
    if (bj_rect_intersection(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;

    ms.x += (int16_t)(inter.x - ds.x);
    ms.y += (int16_t)(inter.y - ds.y);
    ms.w  = inter.w;
    ms.h  = inter.h;
    ds    = inter;
    if (ms.w == 0 || ms.h == 0) return BJ_FALSE;

    // Unpack FG and BG to RGB once
    uint8_t fr, fg, fb;
    uint8_t br, bg, bb;
    bj_make_pixel_rgb(dst->mode, fg_native, &fr, &fg, &fb);
    bj_make_pixel_rgb(dst->mode, bg_native, &br, &bg, &bb);

    // Dispatch to format-specific implementation
    dispatch_blit_mask(mask, &ms, dst, &ds, fg_native, bg_native,
                       fr, fg, fb, br, bg, bb, mode);

    return BJ_TRUE;
}

// ----------------------------------------------------------------------------
// Public API: Stretched mask blit
// ----------------------------------------------------------------------------

bj_bool bj_blit_mask_stretched(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area_in,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area_in,
    uint32_t                fg_native,
    uint32_t                bg_native,
    bj_mask_bg_mode         mode
) {
    bj_check_or_0(mask && dst);

    struct bj_rect ms, ds;
    if (!setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;
    if (ds.w == 0 || ds.h == 0) return BJ_FALSE;

    // Save the *requested* (pre-clip) destination box for proportional mapping
    const struct bj_rect ds_req = ds;

    // Clip destination to surface
    struct bj_rect dst_bounds = (struct bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)min_size_t(dst->width,  UINT16_MAX),
        .h = (uint16_t)min_size_t(dst->height, UINT16_MAX)
    };
    struct bj_rect inter;
    if (bj_rect_intersection(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;
    ds = inter;

    if (ds.w == 0 || ds.h == 0) return BJ_FALSE;

    // Proportional source adjustment for any clipping that happened
    if (ds.x != ds_req.x || ds.y != ds_req.y || ds.w != ds_req.w || ds.h != ds_req.h) {
        // Horizontal
        uint32_t sx0 = 0, sx1 = ms.w;
        if (ds_req.w > 0) {
            const int left_clip  = ds.x - ds_req.x;
            const int right_clip = (ds_req.x + ds_req.w) - (ds.x + ds.w);
            if (left_clip > 0) {
                sx0 = (uint32_t)map_nn((size_t)left_clip, (size_t)ms.w, (size_t)ds_req.w);
            }
            if (right_clip > 0) {
                const uint32_t keep = (uint32_t)ds.w;
                sx1 = (uint32_t)map_nn((size_t)keep, (size_t)ms.w, (size_t)ds_req.w);
                if (sx1 <= sx0) sx1 = sx0 + 1;
                if (sx1 > ms.w) sx1 = ms.w;
            }
        }
        ms.x = (int16_t)(ms.x + (int)sx0);
        ms.w = (uint16_t)(sx1 - sx0);

        // Vertical
        uint32_t sy0 = 0, sy1 = ms.h;
        if (ds_req.h > 0) {
            const int top_clip    = ds.y - ds_req.y;
            const int bottom_clip = (ds_req.y + ds_req.h) - (ds.y + ds.h);
            if (top_clip > 0) {
                sy0 = (uint32_t)map_nn((size_t)top_clip, (size_t)ms.h, (size_t)ds_req.h);
            }
            if (bottom_clip > 0) {
                const uint32_t keep = (uint32_t)ds.h;
                sy1 = (uint32_t)map_nn((size_t)keep, (size_t)ms.h, (size_t)ds_req.h);
                if (sy1 <= sy0) sy1 = sy0 + 1;
                if (sy1 > ms.h) sy1 = ms.h;
            }
        }
        ms.y = (int16_t)(ms.y + (int)sy0);
        ms.h = (uint16_t)(sy1 - sy0);

        if (ms.w == 0 || ms.h == 0) return BJ_FALSE;
    }

    // Unpack FG and BG to RGB once
    uint8_t fr, fg, fb;
    uint8_t br, bg, bb;
    bj_make_pixel_rgb(dst->mode, fg_native, &fr, &fg, &fb);
    bj_make_pixel_rgb(dst->mode, bg_native, &br, &bg, &bb);

    // Dispatch to format-specific implementation
    dispatch_blit_mask_stretched(mask, &ms, dst, &ds, fg_native, bg_native,
                                 fr, fg, fb, br, bg, bb, mode);

    return BJ_TRUE;
}
