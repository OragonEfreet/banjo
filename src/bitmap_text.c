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

    /* Create an 8bpp mask atlas: 0x00 = off, 0xFF = on */
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
                        0xFFu
                    );
                }
            }
        }
    }

    p_bitmap->charset = mask;
    return p_bitmap->charset;
}

void bj_bitmap_blit_text(
    bj_bitmap*   dst,
    int          x,
    int          y,
    unsigned     height,
    uint32_t     color_native,
    const char*  text
) {
    bj_check(dst);
    bj_check(text);
    bj_check(height > 0);

    const bj_bitmap* mask = get_charset_mask(dst);
    bj_check(mask);

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

        bj_bitmap_blit_mask_stretched(
            (const bj_bitmap*)mask, &src_glyph,
            dst, &dest_area,
            color_native
        );

        dest_area.x = (int16_t)(dest_area.x + (int)dest_area.w + spacing);
    }
}

