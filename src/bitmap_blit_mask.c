#include <stdint.h>
#include <stddef.h>

#include <banjo/api.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/math.h>

#include <bitmap.h>
#include <check.h>

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
    const struct bj_bitmap* mask, const struct bj_rect* mask_area_in,
    struct bj_bitmap* dst, const struct bj_rect* dst_area_in,
    struct bj_rect* mask_area, struct bj_rect* dst_area)
{
    bj_check_or_0(mask && dst);

    /* Require one byte per pixel mask */
    if (BJ_PIXEL_GET_BPP(mask->mode) != 8u) return BJ_FALSE;

    struct bj_rect full_mask = (struct bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)bj__min_size_t(mask->width,  UINT16_MAX),
        .h = (uint16_t)bj__min_size_t(mask->height, UINT16_MAX)
    };

    *mask_area = mask_area_in ? *mask_area_in : full_mask;

    struct bj_rect default_dst = (struct bj_rect){ .x = 0, .y = 0, .w = mask_area->w, .h = mask_area->h };
    *dst_area = dst_area_in ? *dst_area_in : default_dst;

    /* Clip mask area to mask bounds */
    struct bj_rect mask_bounds = full_mask;
    if (bj_rect_intersection(&mask_bounds, mask_area, mask_area) == 0) return BJ_FALSE;
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

bj_bool bj_blit_mask(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area_in,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area_in,
    uint32_t         fg_native,
    uint32_t         bg_native,
    bj_mask_bg_mode  mode
) {
    struct bj_rect ms, ds;
    if (!bj__setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;

    /* Non-stretched: sizes must match */
    if (ds.w != ms.w || ds.h != ms.h) return BJ_FALSE;

    /* Clip destination to bounds and adjust source accordingly */
    struct bj_rect dst_bounds = (struct bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX),
        .h = (uint16_t)bj__min_size_t(dst->height, UINT16_MAX)
    };
    struct bj_rect inter;
    if (bj_rect_intersection(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;

    ms.x += inter.x - ds.x; ms.y += inter.y - ds.y;
    ms.w  = inter.w;        ms.h  = inter.h;
    ds    = inter;
    if (ms.w == 0 || ms.h == 0) return BJ_FALSE;

    /* Unpack FG and BG once */
    uint8_t fr, fg, fb;
    uint8_t br, bg, bb;
    bj_make_pixel_rgb(dst->mode, fg_native, &fr, &fg, &fb);
    bj_make_pixel_rgb(dst->mode, bg_native, &br, &bg, &bb);

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
                    bj_put_pixel(dst, dx, dy, fg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, dx, dy);
                    uint8_t dr,dg2,db; bj_make_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a, fr,fg,fb, dr,dg2,db, &or_,&og,&ob);
                    bj_put_pixel(dst, dx, dy, bj_make_bitmap_pixel(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                /* out = mix(BG,FG,a); ignore dst altogether */
                if (a == 0) {
                    bj_put_pixel(dst, dx, dy, bg_native);
                } else if (a == 255) {
                    bj_put_pixel(dst, dx, dy, fg_native);
                } else {
                    const uint8_t or_ = bj__mix_u8(a, fr, br);
                    const uint8_t og  = bj__mix_u8(a, fg, bg);
                    const uint8_t ob  = bj__mix_u8(a, fb, bb);
                    bj_put_pixel(dst, dx, dy, bj_make_bitmap_pixel(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                /* CARVED OUT: BG over dst with alpha (1-a). Leave glyph pixels untouched. */
                const uint8_t a_bg = (uint8_t)(255u - a);
                if (a_bg == 0) {
                    /* fully covered by glyph: keep dst */
                } else if (a_bg == 255) {
                    /* fully outside glyph: paint BG */
                    bj_put_pixel(dst, dx, dy, bg_native);
                } else {
                    /* blend BG over dst with alpha (1-a) */
                    const uint32_t dval = bj_bitmap_pixel(dst, dx, dy);
                    uint8_t dr,dg2,db; bj_make_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a_bg, br,bg,bb, dr,dg2,db, &or_,&og,&ob);
                    bj_put_pixel(dst, dx, dy, bj_make_bitmap_pixel(dst, or_,og,ob));
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

bj_bool bj_blit_mask_stretched(
    const struct bj_bitmap* mask,
    const struct bj_rect*   mask_area_in,
    struct bj_bitmap*       dst,
    const struct bj_rect*   dst_area_in,
    uint32_t         fg_native,
    uint32_t         bg_native,
    bj_mask_bg_mode  mode
) {
    bj_check_or_0(mask && dst);

    struct bj_rect ms, ds;
    if (!bj__setup_mask_rects(mask, mask_area_in, dst, dst_area_in, &ms, &ds))
        return BJ_FALSE;
    if (ds.w == 0 || ds.h == 0) return BJ_FALSE;

    /* Save the *requested* (pre-clip) destination box for proportional mapping */
    const struct bj_rect ds_req = ds;

    /* Clip destination to surface */
    struct bj_rect dst_bounds = (struct bj_rect){
        .x = 0, .y = 0,
        .w = (uint16_t)bj__min_size_t(dst->width,  UINT16_MAX),
        .h = (uint16_t)bj__min_size_t(dst->height, UINT16_MAX)
    };
    struct bj_rect inter;
    if (bj_rect_intersection(&ds, &dst_bounds, &inter) == 0) return BJ_FALSE;
    ds = inter;

    /* If the whole requested box is off-screen, nothing to do */
    if (ds.w == 0 || ds.h == 0) return BJ_FALSE;

    /* Proportional source adjustment for any clipping that happened.
       We want the visible part of the *scaled* glyph to correspond to the same
       portion of the *source* glyph, not to squeeze the whole source into the
       shrunken visible rectangle.
    */
    if (ds.x != ds_req.x || ds.y != ds_req.y || ds.w != ds_req.w || ds.h != ds_req.h) {
        /* Horizontal: compute source subrange [sx0, sx1) kept by the visible dst range */
        uint32_t sx0 = 0, sx1 = ms.w; /* relative to ms.x */
        if (ds_req.w > 0) {
            const int left_clip  = ds.x - ds_req.x;                 /* >=0 if clipped on the left */
            const int right_clip = (ds_req.x + ds_req.w) - (ds.x + ds.w); /* >=0 if clipped on the right */
            if (left_clip > 0) {
                sx0 = (uint32_t)bj__map_nn((size_t)left_clip, (size_t)ms.w, (size_t)ds_req.w);
            }
            if (right_clip > 0) {
                /* number of dst pixels *kept* */
                const uint32_t keep = (uint32_t)ds.w;
                sx1 = (uint32_t)bj__map_nn((size_t)keep, (size_t)ms.w, (size_t)ds_req.w);
                if (sx1 <= sx0) sx1 = sx0 + 1; /* keep at least 1 */
                if (sx1 > ms.w) sx1 = ms.w;
            }
        }
        ms.x = (int16_t)(ms.x + (int)sx0);
        ms.w = (uint16_t)(sx1 - sx0);

        /* Vertical: same idea */
        uint32_t sy0 = 0, sy1 = ms.h;
        if (ds_req.h > 0) {
            const int top_clip    = ds.y - ds_req.y;
            const int bottom_clip = (ds_req.y + ds_req.h) - (ds.y + ds.h);
            if (top_clip > 0) {
                sy0 = (uint32_t)bj__map_nn((size_t)top_clip, (size_t)ms.h, (size_t)ds_req.h);
            }
            if (bottom_clip > 0) {
                const uint32_t keep = (uint32_t)ds.h;
                sy1 = (uint32_t)bj__map_nn((size_t)keep, (size_t)ms.h, (size_t)ds_req.h);
                if (sy1 <= sy0) sy1 = sy0 + 1;
                if (sy1 > ms.h) sy1 = ms.h;
            }
        }
        ms.y = (int16_t)(ms.y + (int)sy0);
        ms.h = (uint16_t)(sy1 - sy0);

        if (ms.w == 0 || ms.h == 0) return BJ_FALSE;
    }

    /* Unpack FG and BG once (dst-native) */
    uint8_t fr, fg, fb;
    uint8_t br, bg, bb;
    bj_make_pixel_rgb(dst->mode, fg_native, &fr, &fg, &fb);
    bj_make_pixel_rgb(dst->mode, bg_native, &br, &bg, &bb);

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
                    bj_put_pixel(dst, out_x, out_y, fg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, out_x, out_y);
                    uint8_t dr,dg2,db; bj_make_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a, fr,fg,fb, dr,dg2,db, &or_,&og,&ob);
                    bj_put_pixel(dst, out_x, out_y, bj_make_bitmap_pixel(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_OPAQUE:
                if (a == 0) {
                    bj_put_pixel(dst, out_x, out_y, bg_native);
                } else if (a == 255) {
                    bj_put_pixel(dst, out_x, out_y, fg_native);
                } else {
                    const uint8_t or_ = bj__mix_u8(a, fr, br);
                    const uint8_t og  = bj__mix_u8(a, fg, bg);
                    const uint8_t ob  = bj__mix_u8(a, fb, bb);
                    bj_put_pixel(dst, out_x, out_y, bj_make_bitmap_pixel(dst, or_,og,ob));
                }
                break;

            case BJ_MASK_BG_REV_TRANSPARENT: {
                const uint8_t a_bg = (uint8_t)(255u - a);
                if (a_bg == 0) { /* inside glyph → keep dst */ }
                else if (a_bg == 255) {
                    bj_put_pixel(dst, out_x, out_y, bg_native);
                } else {
                    const uint32_t dval = bj_bitmap_pixel(dst, out_x, out_y);
                    uint8_t dr,dg2,db; bj_make_pixel_rgb(dst->mode, dval, &dr,&dg2,&db);
                    uint8_t or_,og,ob;  bj__src_over_rgb(a_bg, br,bg,bb, dr,dg2,db, &or_,&og,&ob);
                    bj_put_pixel(dst, out_x, out_y, bj_make_bitmap_pixel(dst, or_,og,ob));
                }
            } break;
            }
        }
    }

    return BJ_TRUE;
}
