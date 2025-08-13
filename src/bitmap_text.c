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

/* Forward decl: we’ll use the cached per-bitmap charset atlas */
static const bj_bitmap* get_charset(bj_bitmap* p_bitmap);

/* ---------------- Internal: build or return cached charset atlas ---------------- */

static const bj_bitmap* get_charset(bj_bitmap* p_bitmap)
{
    bj_check_or_0(p_bitmap);

    if (p_bitmap->charset) {
        return p_bitmap->charset;
    }

    bj_trace("Preparing bitmap charset");

    const size_t charset_char_len     = sizeof charset_latin1 / sizeof charset_latin1[0];
    const size_t charset_rows         = (charset_char_len + CHARSET_CHAR_PER_ROW - 1) / CHARSET_CHAR_PER_ROW;
    const size_t charset_pixel_width  = (size_t)CHARSET_CHAR_PER_ROW * (size_t)CHAR_PIXEL_W;
    const size_t charset_pixel_height = charset_rows * (size_t)CHAR_PIXEL_H;

    /* Create atlas in the SAME mode as the destination to avoid per-pixel conversion */
    bj_bitmap* charset = bj_bitmap_new(
        charset_pixel_width, charset_pixel_height,
        (bj_pixel_mode)bj_bitmap_mode(p_bitmap), 0
    );
    bj_check_or_0(charset);

    /* Pack colors in the charset's native format */
    const uint32_t white_native = bj_bitmap_pixel_value(charset, 0xFF, 0xFF, 0xFF);
    const uint32_t black_native = bj_bitmap_pixel_value(charset, 0x00, 0x00, 0x00);

    /* Clear to black */
    bj_bitmap_set_clear_color(charset, black_native);
    bj_bitmap_clear(charset);

    /* Paint white bits for each glyph */
    for (size_t idx = 0; idx < charset_char_len; ++idx) {
        const int base_x = CHAR_PIXEL_X((int)idx);
        const int base_y = CHAR_PIXEL_Y((int)idx);

        for (int row = 0; row < CHAR_PIXEL_H; ++row) {
            const uint8_t rowbits = charset_latin1[idx][row];
            for (int col = 0; col < CHAR_PIXEL_W; ++col) {
                if ((rowbits >> col) & 0x01u) {
                    bj_bitmap_put_pixel(charset, (size_t)(base_x + col), (size_t)(base_y + row), white_native);
                }
            }
        }
    }

    /* Make black background transparent for blitting */
    bj_bitmap_set_colorkey(charset, BJ_TRUE, black_native);

    p_bitmap->charset = charset;
    return p_bitmap->charset;
}

/* ---------------- Public: blit ASCII text using the cached atlas ---------------- */

void bj_bitmap_blit_text(
    bj_bitmap*    p_bitmap,
    int           x,
    int           y,
    unsigned int  height,
    const char*   text
) {
    bj_check(p_bitmap);
    bj_check(text);
    bj_check(height > 0);

    const bj_bitmap* charset = get_charset(p_bitmap);
    bj_check(charset);

    /* Scale 8x8 glyph to requested height, keep aspect ratio (integer math) */
    const uint16_t glyph_w = (uint16_t)((height * CHAR_PIXEL_W + CHAR_PIXEL_H/2) / CHAR_PIXEL_H);
    const uint16_t glyph_h = (uint16_t)height;

    int spacing = (int)bj_round(0.1 * (double)CHAR_PIXEL_W);
    if (spacing < 1) spacing = 1;

    const size_t table_len = sizeof charset_latin1 / sizeof charset_latin1[0];
    const size_t len = bj_strlen(text);

    bj_rect dest_area;
    dest_area.x = x;
    dest_area.y = y;
    dest_area.w = glyph_w;
    dest_area.h = glyph_h;

    for (size_t i = 0; i < len; ++i) {
        uint8_t code = (uint8_t)text[i];
        if (code >= table_len) {
            /* Fallback to '?' if outside table */
            code = (uint8_t)'?';
        }

        const bj_rect char_area = {
            .x = (int16_t)CHAR_PIXEL_X((int)code),
            .y = (int16_t)CHAR_PIXEL_Y((int)code),
            .w = (uint16_t)CHAR_PIXEL_W,
            .h = (uint16_t)CHAR_PIXEL_H
        };

        /* Stretched blit from atlas → dst. The blitter should honor the charset’s colorkey. */
        bj_bitmap_blit_stretched((const bj_bitmap*)charset, &char_area, p_bitmap, &dest_area);

        dest_area.x = (int16_t)(dest_area.x + (int)dest_area.w + spacing);
    }
}

/* ---------------- Public: render colored text directly from the 1-bit mask ---------------- */
/* This avoids the atlas and writes the chosen color straight to the destination. */

void bj_bitmap_blit_text_masked_color(
    bj_bitmap*   dst,
    int          x,
    int          y,
    unsigned     height,
    uint32_t     color_native,   /* packed in dst’s native pixel format */
    const char*  text
) {
    bj_check(dst);
    bj_check(text);
    bj_check(height > 0);

    const size_t len = bj_strlen(text);
    const uint16_t glyph_w = (uint16_t)((height * CHAR_PIXEL_W + CHAR_PIXEL_H/2) / CHAR_PIXEL_H);
    const uint16_t glyph_h = (uint16_t)height;

    int spacing = (int)bj_round(0.1 * (double)CHAR_PIXEL_W);
    if (spacing < 1) spacing = 1;

    const size_t table_len = sizeof charset_latin1 / sizeof charset_latin1[0];

    for (size_t i = 0; i < len; ++i) {
        uint8_t code = (uint8_t)text[i];
        if (code >= table_len) code = (uint8_t)'?';

        for (uint16_t dy = 0; dy < glyph_h; ++dy) {
            /* Nearest neighbor: map destination row to source 0..7 */
            const size_t sy = (size_t)dy * CHAR_PIXEL_H / glyph_h;
            const uint8_t rowbits = charset_latin1[code][sy];
            const int dst_y = y + (int)dy;
            if ((unsigned)dst_y >= dst->height) continue;

            for (uint16_t dx = 0; dx < glyph_w; ++dx) {
                const size_t sx = (size_t)dx * CHAR_PIXEL_W / glyph_w; /* 0..7 */
                if ((rowbits >> sx) & 1u) {
                    const int dst_x = x + (int)dx;
                    if ((unsigned)dst_x < dst->width) {
                        bj_bitmap_put_pixel(dst, (size_t)dst_x, (size_t)dst_y, color_native);
                    }
                }
            }
        }

        x += (int)glyph_w + spacing;
    }
}
