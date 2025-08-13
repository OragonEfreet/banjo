#include <stdint.h>
#include <stddef.h>

#include <banjo/api.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/math.h>

#include "bitmap_t.h"
#include "check.h"

/* -------------------------------------------------------------------------- */
/* Helpers                                                                    */
/* -------------------------------------------------------------------------- */

static inline size_t bj__min_size_t(size_t a, size_t b) { return (a < b) ? a : b; }

/* Map dst i in [0..dst_len) to src in [0..src_len) (nearest neighbor) */
static inline size_t bj__map_nn(size_t i, size_t src_len, size_t dst_len) {
    return (dst_len == 0) ? 0 : (size_t)((uint64_t)i * (uint64_t)src_len / (uint64_t)dst_len);
}

/* Validate & prepare rectangles (mask must be 8 bpp) */
static bj_bool bj__setup_mask_rects(
    const bj_bitmap* mask, const bj_rect* mask_area_in,
    bj_bitmap* dst, const bj_rect* dst_area_in,
    bj_rect* mask_area, bj_rect* dst_area)
{
    bj_check_or_0(mask && dst);

    /* Require one byte per pixel mask */
    if (BJ_PIXEL_GET_BPP(mask->mode) != 8u) return BJ_FALSE;

    bj_rect full_mask = (bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)bj__min_size_t(mask->width,  UINT16_MAX),
        .h = (uint16_t)bj__min_size_t(mask->height, UINT16_MAX)
    };

    *mask_area = mask_area_in ? *mask_area_in : full_mask;

    bj_rect default_dst = (bj_rect){ .x = 0, .y = 0, .w = mask_area->w, .h = mask_area->h };
    *dst_area = dst_area_in ? *dst_area_in : default_dst;

    /* Clip mask area to mask bounds */
    bj_rect mask_bounds = full_mask;
    if (bj_rect_intersect(&mask_bounds, mask_area, mask_area) == 0) return BJ_FALSE;
    if (mask_area->w == 0 || mask_area->h == 0) return BJ_FALSE;

    return BJ_TRUE;
}

/* Integer alpha blend (0..255) */
static inline uint8_t bj__mix_u8(uint16_t a, uint8_t s, uint8_t d) {
    /* (d*(255-a) + s*a + 127) / 255 with rounding */
    return (uint8_t)(((uint16_t)d * (255u - a) + (uint16_t)s * a + 127u) / 255u);
}

static inline void bj__src_over_rgb(
    uint8_t a,
    uint8_t sr, uint8_t sg, uint8_t sb,
    uint8_t dr, uint8_t dg, uint8_t db,
    uint8_t* or_, uint8_t* og, uint8_t* ob)
{
    *or_ = bj__mix_u8(a, sr, dr);
    *og  = bj__mix_u8(a, sg, dg);
    *ob  = bj__mix_u8(a, sb, db);
}

/* -------------------------------------------------------------------------- */
/* Non-stretched mask blit                                                    */
/* -------------------------------------------------------------------------- */

bj_bool bj_bitmap_blit_mask(
    const bj_bitmap* mask,
    const bj_rect*   mask_area_in,
    bj_bitmap*       dst,
    const bj_rect*   dst_area_in,
    uint32_t         fg_native,
    uint32_t         bg_native,
    bj_mask_bg_mode  mode
) {
    bj_rect ms, ds;
    if (!bj__setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;

    /* Non-stretched: sizes must match */
    if (ds.w != ms.w || ds.h != ms.h) return BJ_FALSE;

    /* Clip destination to bounds and adjust source accordingly */
    bj_rect dst_bounds = (bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX),
        .h = (uint16_t)bj__min_size_t(dst->height, UINT16_MAX)
    };
    bj_rect inter;
    if (bj_rect_intersect(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;

    ms.x += inter.x - ds.x; ms.y += inter.y - ds.y;
    ms.w  = inter.w;        ms.h  = inter.h;
    ds    = inter;
    if (ms.w == 0 || ms.h == 0) return BJ_FALSE;

    /* Unpack FG and BG once */
    uint8_t fr, fg, fb;
    uint8_t br, bg, bb;
    bj_pixel_rgb(dst->mode, fg_native, &fr, &fg, &fb);
    bj_pixel_rgb(dst->mode, bg_native, &br, &bg, &bb);

    for (uint16_t r = 0; r < ds.h; ++r) {
        const size_t my = (size_t)ms.y + r;
        const size_t dy = (size_t)ds.y + r;
        const uint8_t* mrow = (const uint8_t*)mask->buffer + my * mask->stride;

        for (uint16_t c = 0; c < ds.w; ++c) {
            const size_t mx = (size_t)ms.x + c;
            const size_t dx = (size_t)ds.x + c;

            const uint8_t a = mrow[mx]; /* glyph coverage (0..255) */

            switch (mode) {
            case BJ_MASK_BG_TRANSPARENT:
                /* FG over dst where a>0; keep dst where a==0 */
                if (a == 0) continue;
                if (a == 255) {
                    bj_bitmap_put_pixel(dst, dx, dy, fg_native);
                } else {
                    const uint32_t dval = bj_bitmap_get(dst, dx, dy);
                    uint8_t dr,dg2,db; bj_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a, fr,fg,fb, dr,dg2,db, &or_,&og,&ob);
                    bj_bitmap_put_pixel(dst, dx, dy, bj_bitmap_pixel_value(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                /* out = mix(BG,FG,a); ignore dst altogether */
                if (a == 0) {
                    bj_bitmap_put_pixel(dst, dx, dy, bg_native);
                } else if (a == 255) {
                    bj_bitmap_put_pixel(dst, dx, dy, fg_native);
                } else {
                    const uint8_t or_ = bj__mix_u8(a, fr, br);
                    const uint8_t og  = bj__mix_u8(a, fg, bg);
                    const uint8_t ob  = bj__mix_u8(a, fb, bb);
                    bj_bitmap_put_pixel(dst, dx, dy, bj_bitmap_pixel_value(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                /* CARVED OUT: BG over dst with alpha (1-a). Leave glyph pixels untouched. */
                const uint8_t a_bg = (uint8_t)(255u - a);
                if (a_bg == 0) {
                    /* fully covered by glyph: keep dst */
                } else if (a_bg == 255) {
                    /* fully outside glyph: paint BG */
                    bj_bitmap_put_pixel(dst, dx, dy, bg_native);
                } else {
                    /* blend BG over dst with alpha (1-a) */
                    const uint32_t dval = bj_bitmap_get(dst, dx, dy);
                    uint8_t dr,dg2,db; bj_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a_bg, br,bg,bb, dr,dg2,db, &or_,&og,&ob);
                    bj_bitmap_put_pixel(dst, dx, dy, bj_bitmap_pixel_value(dst, or_,og,ob));
                }
            } break;
            }
        }
    }

    return BJ_TRUE;
}

/* -------------------------------------------------------------------------- */
/* Stretched mask blit (nearest neighbor)                                     */
/* -------------------------------------------------------------------------- */

bj_bool bj_bitmap_blit_mask_stretched(
    const bj_bitmap* mask,
    const bj_rect*   mask_area_in,
    bj_bitmap*       dst,
    const bj_rect*   dst_area_in,
    uint32_t         fg_native,
    uint32_t         bg_native,
    bj_mask_bg_mode  mode
) {
    bj_check_or_0(mask && dst);

    bj_rect ms, ds;
    if (!bj__setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;

    if (!dst_area_in) {
        ds.x = 0; ds.y = 0;
        ds.w = (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX);
        ds.h = (uint16_t)bj__min_size_t(dst->height, UINT16_MAX);
    }
    if (ds.w == 0 || ds.h == 0) return BJ_FALSE;

    /* Clip destination */
    bj_rect dst_bounds = (bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX),
        .h = (uint16_t)bj__min_size_t(dst->height, UINT16_MAX)
    };
    bj_rect inter;
    if (bj_rect_intersect(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;
    ds = inter;

    /* Unpack FG and BG once */
    uint8_t fr, fg, fb;
    uint8_t br, bg, bb;
    bj_pixel_rgb(dst->mode, fg_native, &fr, &fg, &fb);
    bj_pixel_rgb(dst->mode, bg_native, &br, &bg, &bb);

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

            switch (mode) {
            case BJ_MASK_BG_TRANSPARENT:
                if (a == 0) continue;
                if (a == 255) {
                    bj_bitmap_put_pixel(dst, out_x, out_y, fg_native);
                } else {
                    const uint32_t dval = bj_bitmap_get(dst, out_x, out_y);
                    uint8_t dr,dg2,db; bj_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a, fr,fg,fb, dr,dg2,db, &or_,&og,&ob);
                    bj_bitmap_put_pixel(dst, out_x, out_y, bj_bitmap_pixel_value(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                if (a == 0) {
                    bj_bitmap_put_pixel(dst, out_x, out_y, bg_native);
                } else if (a == 255) {
                    bj_bitmap_put_pixel(dst, out_x, out_y, fg_native);
                } else {
                    const uint8_t or_ = bj__mix_u8(a, fr, br);
                    const uint8_t og  = bj__mix_u8(a, fg, bg);
                    const uint8_t ob  = bj__mix_u8(a, fb, bb);
                    bj_bitmap_put_pixel(dst, out_x, out_y, bj_bitmap_pixel_value(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                /* CARVED OUT: BG over dst with alpha (1-a) */
                const uint8_t a_bg = (uint8_t)(255u - a);
                if (a_bg == 0) {
                    /* inside glyph: keep dst */
                } else if (a_bg == 255) {
                    /* outside glyph: paint BG */
                    bj_bitmap_put_pixel(dst, out_x, out_y, bg_native);
                } else {
                    const uint32_t dval = bj_bitmap_get(dst, out_x, out_y);
                    uint8_t dr,dg2,db; bj_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a_bg, br,bg,bb, dr,dg2,db, &or_,&og,&ob);
                    bj_bitmap_put_pixel(dst, out_x, out_y, bj_bitmap_pixel_value(dst, or_,og,ob));
                }
            } break;
            }
        }
    }

    return BJ_TRUE;
}
