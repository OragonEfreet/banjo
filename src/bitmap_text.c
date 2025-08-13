// Note: This file is AI-generated.
// TODO: I'll remake it myself later on.

#include <stdint.h>
#include <stddef.h>

#include <banjo/log.h>
#include <banjo/math.h>
#include <banjo/string.h>

#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/bitmap.h>

#include "bitmap_charsets.h"
#include "bitmap_t.h"
#include "check.h"

#define CHARSET_CHAR_PER_ROW 16
#define CHAR_PIXEL_X(c) ((int)((c) % CHARSET_CHAR_PER_ROW) * CHAR_PIXEL_W)
#define CHAR_PIXEL_Y(c) ((int)((c) / CHARSET_CHAR_PER_ROW) * CHAR_PIXEL_H)

/* =========================
   8bpp glyph mask atlas
   ========================= */
static const bj_bitmap* get_charset_mask(bj_bitmap* p_bitmap)
{
    bj_check_or_0(p_bitmap);

    if (p_bitmap->charset) {
        return p_bitmap->charset;
    }

    bj_trace("Preparing bitmap charset (8-bit mask)");

    const size_t charset_char_len     = sizeof charset_latin1 / sizeof charset_latin1[0];
    const size_t charset_rows         = (charset_char_len + CHARSET_CHAR_PER_ROW - 1u) / CHARSET_CHAR_PER_ROW;
    const size_t charset_pixel_width  = (size_t)CHARSET_CHAR_PER_ROW * (size_t)CHAR_PIXEL_W;
    const size_t charset_pixel_height = charset_rows * (size_t)CHAR_PIXEL_H;

    bj_bitmap* mask = bj_bitmap_new(
        charset_pixel_width, charset_pixel_height,
        BJ_PIXEL_MODE_INDEXED_8, 0
    );
    bj_check_or_0(mask);

    bj_bitmap_set_clear_color(mask, 0x00u);
    bj_bitmap_clear(mask);

    for (size_t idx = 0; idx < charset_char_len; ++idx) {
        const int base_x = CHAR_PIXEL_X((int)idx);
        const int base_y = CHAR_PIXEL_Y((int)idx);

        for (int row = 0; row < CHAR_PIXEL_H; ++row) {
            const uint8_t rowbits = charset_latin1[idx][row];
            for (int col = 0; col < CHAR_PIXEL_W; ++col) {
                if ((rowbits >> col) & 0x01u) {
                    bj_bitmap_put_pixel(
                        mask,
                        (size_t)(base_x + col),
                        (size_t)(base_y + row),
                        0xFFu /* full coverage */
                    );
                }
            }
        }
    }

    p_bitmap->charset = mask;
    return p_bitmap->charset;
}

/* =========================
   Fast fill (row-wise, no scratch, C99)
   ========================= */
static void fast_fill_rect(bj_bitmap* dst, const bj_rect* r, uint32_t color_native)
{
    bj_check(dst);
    bj_check(r);
    if (r->w == 0 || r->h == 0) return;

    int x0 = r->x, y0 = r->y;
    int x1 = x0 + (int)r->w, y1 = y0 + (int)r->h;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (int)dst->width)  x1 = (int)dst->width;
    if (y1 > (int)dst->height) y1 = (int)dst->height;
    if (x0 >= x1 || y0 >= y1) return;

    const size_t bpp = BJ_PIXEL_GET_BPP(dst->mode);
    /* If not byte-aligned (e.g., 1/4 bpp), fallback to per-pixel put. */
    if ((bpp & 7u) != 0u) {
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                bj_bitmap_put_pixel(dst, (size_t)x, (size_t)y, color_native);
            }
        }
        return;
    }

    const size_t bytes_per_px = bpp >> 3;

    for (int y = y0; y < y1; ++y) {
        /* Seed first pixel via API to ensure correct native packing. */
        bj_bitmap_put_pixel(dst, (size_t)x0, (size_t)y, color_native);

        /* Then double the written region using in-place memcpy. */
        uint8_t* row_start = (uint8_t*)dst->buffer + (size_t)y * dst->stride + (size_t)x0 * bytes_per_px;
        size_t filled = bytes_per_px;                                 /* bytes already valid in row */
        const size_t row_bytes = (size_t)(x1 - x0) * bytes_per_px;    /* total bytes to fill */

        while ((filled << 1) <= row_bytes) {
            bj_memcpy(row_start + filled, row_start, filled);
            filled <<= 1;
        }
        if (filled < row_bytes) {
            bj_memcpy(row_start + filled, row_start, row_bytes - filled);
        }
    }
}

/* =========================
   Text renderer
   ========================= */
static void render_text_masked(
    bj_bitmap*      dst,
    int             x,
    int             y,
    unsigned        height,
    uint32_t        fg_native,
    uint32_t        bg_native,
    bj_mask_bg_mode mode,
    const char*     text
) {
    bj_check(dst);
    bj_check(text);
    bj_check(height > 0);

    const bj_bitmap* mask = get_charset_mask(dst);
    bj_check(mask);

    /* Scale glyph box from CHAR_PIXEL_W×CHAR_PIXEL_H to requested pixel height (keep aspect) */
    const uint16_t glyph_w = (uint16_t)((height * CHAR_PIXEL_W + CHAR_PIXEL_H/2) / CHAR_PIXEL_H);
    const uint16_t glyph_h = (uint16_t)height;

    int spacing = (int)bj_round(0.1 * (double)CHAR_PIXEL_W);
    if (spacing < 1) spacing = 1;

    const size_t table_len = sizeof charset_latin1 / sizeof charset_latin1[0];
    const size_t len = bj_strlen(text);

    bj_rect dest_area = { (int16_t)x, (int16_t)y, glyph_w, glyph_h };

    for (size_t i = 0; i < len; ++i) {
        uint8_t code = (uint8_t)text[i];
        if (code >= table_len) code = (uint8_t)'?';

        const bj_rect src_glyph = {
            .x = (int16_t)CHAR_PIXEL_X((int)code),
            .y = (int16_t)CHAR_PIXEL_Y((int)code),
            .w = (uint16_t)CHAR_PIXEL_W,
            .h = (uint16_t)CHAR_PIXEL_H
        };

        /* Correct function: FG+BG+mode mask blit */
        bj_bitmap_blit_mask_stretched(
            mask, &src_glyph,
            dst, &dest_area,
            fg_native, bg_native, mode
        );

        /* Bridge the spacing gap with BG when carving so backgrounds touch */
        if (mode == BJ_MASK_BG_REV_TRANSPARENT && spacing > 0) {
            bj_rect gap = {
                .x = (int16_t)(dest_area.x + (int)dest_area.w),
                .y = dest_area.y,
                .w = (uint16_t)spacing,
                .h = dest_area.h
            };
            fast_fill_rect(dst, &gap, bg_native);
        }

        dest_area.x = (int16_t)(dest_area.x + (int)dest_area.w + spacing);
    }
}

/* Public: FG+BG with selectable background mode */
void bj_bitmap_blit_text(
    bj_bitmap*      dst,
    int             x,
    int             y,
    unsigned        height,
    uint32_t        fg_native,
    uint32_t        bg_native,
    bj_mask_bg_mode mode,            /* TRANSPARENT / OPAQUE / REV_TRANSPARENT */
    const char*     text
) {
    render_text_masked(dst, x, y, height, fg_native, bg_native, mode, text);
}
