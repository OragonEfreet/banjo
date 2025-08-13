// Note: This file is AI-generated.
// TODO: I'll remake it myself later on.

#include <banjo/api.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/math.h>

#include <stdint.h>
#include <stddef.h>

#include "bitmap_t.h"
#include "check.h"

/* ---- Helpers ---------------------------------------------------------------- */

static inline size_t bj__min_size_t(size_t a, size_t b) { return (a < b) ? a : b; }
static inline size_t bj__map_nn(size_t i, size_t src_len, size_t dst_len) {
    return (dst_len == 0) ? 0 : (size_t)((uint64_t)i * (uint64_t)src_len / (uint64_t)dst_len);
}

/* Clamp/validate rect, build defaults, and compute intersections like your blit. */
static bj_bool bj__setup_mask_rects(
    const bj_bitmap* mask, const bj_rect* mask_area_in,
    bj_bitmap* dst, const bj_rect* dst_area_in,
    bj_rect* mask_area, bj_rect* dst_area)
{
    bj_check_or_0(mask && dst);

    /* Mask must be 8 bpp (one byte per pixel) */
    if (BJ_PIXEL_GET_BPP(mask->mode) != 8u) {
        return BJ_FALSE;
    }

    /* Default mask_area = full mask */
    bj_rect full_mask = {0, 0, (uint16_t)bj__min_size_t(mask->width,  UINT16_MAX),
                              (uint16_t)bj__min_size_t(mask->height, UINT16_MAX)};
    *mask_area = mask_area_in ? *mask_area_in : full_mask;

    /* Default dst_area = place at (0,0) same size as mask_area */
    bj_rect default_dst = {0, 0, mask_area->w, mask_area->h};
    *dst_area = dst_area_in ? *dst_area_in : default_dst;

    /* Clip mask_area to mask bounds */
    bj_rect mask_bounds = {0, 0, (uint16_t)bj__min_size_t(mask->width,  UINT16_MAX),
                               (uint16_t)bj__min_size_t(mask->height, UINT16_MAX)};
    if (bj_rect_intersect(&mask_bounds, mask_area, mask_area) == 0) return BJ_FALSE;
    if (mask_area->w == 0 || mask_area->h == 0) return BJ_FALSE;

    /* If not stretching (equal sizes expected), caller will handle; for stretched, sizes may differ. */
    return BJ_TRUE;
}

/* Read a mask byte quickly (mask bpp = 8 guaranteed). */
static inline uint8_t bj__mask_byte_at(const bj_bitmap* mask, size_t x, size_t y) {
    const uint8_t* row = (const uint8_t*)mask->buffer + y * mask->stride;
    return row[x];
}

/* Blend: out = dst*(255-a)/255 + src*a/255, per channel (integer) */
static inline void bj__blend_src_over_u8(
    uint8_t a, uint8_t sr, uint8_t sg, uint8_t sb,
    uint8_t dr, uint8_t dg, uint8_t db,
    uint8_t* or_, uint8_t* og, uint8_t* ob)
{
    /* Use 16-bit math to avoid overflow: ((d*(255-a)) + (s*a) + 127) / 255 with rounding */
    const uint16_t ia = (uint16_t)(255u - a);
    uint16_t r = (uint16_t)dr * ia + (uint16_t)sr * a;
    uint16_t g = (uint16_t)dg * ia + (uint16_t)sg * a;
    uint16_t b = (uint16_t)db * ia + (uint16_t)sb * a;

    /* approximate /255 with ((x + 127) * 257) >> 16 (good integer approximation), or simple divide. */
    *or_ = (uint8_t)((r + 127u) / 255u);
    *og  = (uint8_t)((g + 127u) / 255u);
    *ob  = (uint8_t)((b + 127u) / 255u);
}

/* ---- Non-stretched mask blit ----------------------------------------------- */

bj_bool bj_bitmap_blit_mask(
    const bj_bitmap* mask,
    const bj_rect*   mask_area_in,
    bj_bitmap*       dst,
    const bj_rect*   dst_area_in,
    uint32_t         color_native
) {
    bj_rect ms, ds;
    if (!bj__setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;

    /* If sizes differ, refuse here (use stretched variant) */
    if (ds.w != ms.w || ds.h != ms.h) return BJ_FALSE;

    /* Clip destination to bounds and adjust mask accordingly */
    bj_rect dst_bounds = (bj_rect){0, 0, (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX),
                                       (uint16_t)bj__min_size_t(dst->height, UINT16_MAX)};
    bj_rect inter;
    if (bj_rect_intersect(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;

    ms.x += inter.x - ds.x;
    ms.y += inter.y - ds.y;
    ms.w  = inter.w;
    ms.h  = inter.h;
    ds    = inter;

    if (ms.w == 0 || ms.h == 0) return BJ_FALSE;

    /* Pre-unpack the tint color once (RGB) */
    uint8_t sr=0, sg=0, sb=0;
    bj_pixel_rgb(dst->mode, color_native, &sr, &sg, &sb);

    for (uint16_t r = 0; r < ds.h; ++r) {
        const size_t my = (size_t)ms.y + r;
        const size_t dy = (size_t)ds.y + r;

        const uint8_t* mrow = (const uint8_t*)mask->buffer + my * mask->stride;

        for (uint16_t c = 0; c < ds.w; ++c) {
            const size_t mx = (size_t)ms.x + c;
            const size_t dx = (size_t)ds.x + c;

            const uint8_t a = mrow[mx];
            if (a == 0) continue;
            if (a == 255) {
                /* Opaque: write color directly */
                bj_bitmap_put_pixel(dst, dx, dy, color_native);
                continue;
            }

            /* General blend: fetch dst, convert, blend, pack back */
            uint32_t dval = bj_bitmap_get(dst, dx, dy);
            uint8_t dr, dg, db;
            bj_pixel_rgb(dst->mode, dval, &dr, &dg, &db);

            uint8_t or_, og, ob;
            bj__blend_src_over_u8(a, sr, sg, sb, dr, dg, db, &or_, &og, &ob);

            uint32_t out = bj_bitmap_pixel_value(dst, or_, og, ob);
            bj_bitmap_put_pixel(dst, dx, dy, out);
        }
    }
    return BJ_TRUE;
}

/* ---- Stretched mask blit (nearest neighbor) -------------------------------- */

bj_bool bj_bitmap_blit_mask_stretched(
    const bj_bitmap* mask,
    const bj_rect*   mask_area_in,
    bj_bitmap*       dst,
    const bj_rect*   dst_area_in,
    uint32_t         color_native
) {
    bj_check_or_0(mask && dst);

    /* Setup and clip source (mask) rect */
    bj_rect ms, ds;
    if (!bj__setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;

    /* Default dst_area if none provided = full dst */
    if (!dst_area_in) {
        ds.x = 0; ds.y = 0;
        ds.w = (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX);
        ds.h = (uint16_t)bj__min_size_t(dst->height, UINT16_MAX);
    }

    if (ds.w == 0 || ds.h == 0) return BJ_FALSE;

    /* Clip destination, then we'll map back to ms with NN */
    bj_rect dst_bounds = (bj_rect){0, 0, (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX),
                                       (uint16_t)bj__min_size_t(dst->height, UINT16_MAX)};
    bj_rect inter;
    if (bj_rect_intersect(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;
    ds = inter;

    /* Pre-unpack the tint color once (RGB) */
    uint8_t sr=0, sg=0, sb=0;
    bj_pixel_rgb(dst->mode, color_native, &sr, &sg, &sb);

    const size_t sw = ms.w, sh = ms.h;
    const size_t dw = ds.w, dh = ds.h;

    for (uint16_t dy = 0; dy < dh; ++dy) {
        const size_t sy = (size_t)ms.y + bj__map_nn(dy, sh, dh);
        const size_t out_y = (size_t)ds.y + dy;

        const uint8_t* mrow = (const uint8_t*)mask->buffer + sy * mask->stride;

        for (uint16_t dx = 0; dx < dw; ++dx) {
            const size_t sx = (size_t)ms.x + bj__map_nn(dx, sw, dw);
            const size_t out_x = (size_t)ds.x + dx;

            const uint8_t a = mrow[sx];
            if (a == 0) continue;
            if (a == 255) {
                bj_bitmap_put_pixel(dst, out_x, out_y, color_native);
                continue;
            }

            uint32_t dval = bj_bitmap_get(dst, out_x, out_y);
            uint8_t dr, dg, db;
            bj_pixel_rgb(dst->mode, dval, &dr, &dg, &db);

            uint8_t or_, og, ob;
            bj__blend_src_over_u8(a, sr, sg, sb, dr, dg, db, &or_, &og, &ob);

            uint32_t out = bj_bitmap_pixel_value(dst, or_, og, ob);
            bj_bitmap_put_pixel(dst, out_x, out_y, out);
        }
    }

    return BJ_TRUE;
}
