#include <banjo/log.h>

#include "bitmap_charsets.h"
#include "bitmap_t.h"
#include "check.h"

#define CHARSET_CHAR_PER_ROW 16
#define CHAR_PIXEL_X(c) ((c % CHARSET_CHAR_PER_ROW) * CHAR_PIXEL_W)
#define CHAR_PIXEL_Y(c) ((c / CHARSET_CHAR_PER_ROW) * CHAR_PIXEL_H)

static const bj_bitmap* get_charset(
    bj_bitmap* p_bitmap
) {
    bj_check_or_0(p_bitmap);

    if(p_bitmap->charset != 0) {
        return p_bitmap->charset;
    }

    bj_trace("Preparing bitmap charset");

    const size_t charset_char_len     = sizeof charset_latin1 / sizeof charset_latin1[0];
    const size_t charset_char_per_col = charset_char_len / CHARSET_CHAR_PER_ROW + (charset_char_len % CHARSET_CHAR_PER_ROW > 0);
    const size_t charset_pixel_width  = CHARSET_CHAR_PER_ROW * CHAR_PIXEL_W;
    const size_t charset_pixel_height = charset_char_per_col * CHAR_PIXEL_H;
    
    /* bj_trace("Charset number of chars: %d/%d", CHARSET_CHAR_PER_ROW, charset_char_per_col); */
    /* bj_trace("Charset size in pixels: %d/%d", charset_width, charset_height); */

    bj_bitmap* charset = bj_bitmap_new(
        charset_pixel_width, charset_pixel_height,
        bj_bitmap_mode(p_bitmap), 0
    );

    const uint32_t white = bj_bitmap_pixel_value(charset, 0xFF, 0xFF, 0xFF);

    for(size_t c = 0 ; c < charset_char_len ; ++c) {
        size_t x = CHAR_PIXEL_X(c);
        size_t y = CHAR_PIXEL_Y(c);

        for(size_t row = 0 ; row < CHAR_PIXEL_H ; ++row) {
            for(size_t column = 0 ; column < CHAR_PIXEL_W ; ++column) {
                if((charset_latin1[c][row] >> column) & 0x01) {
                    bj_bitmap_put_pixel(charset, x + column, y + row, white);
                }

            }
        }
    }

    p_bitmap->charset = charset;
    return p_bitmap->charset;
}

void bj_bitmap_blit_text(
    bj_bitmap* p_bitmap,
    int _x,
    int _y,
    unsigned int height,
    const char* text
) {
    bj_check(p_bitmap);
    bj_check(height > 0);
    bj_check(text);
    /* bj_debug("blit text \"%s\" at {%d, %d}, size %ud", text, x, y, height); */

    const bj_bitmap* charset = get_charset(p_bitmap);

    const char c = 'T';

    const bj_rect char_area = {
        .x = CHAR_PIXEL_X(c), .y = CHAR_PIXEL_Y(c),
        .w = CHAR_PIXEL_W,    .h = CHAR_PIXEL_H,
    };

    const bj_rect dest_area = {
        .x = 100, .y = 100,
        .w = 48,  .h = 48,
    };

    bj_bitmap_blit_stretched(charset, &char_area, p_bitmap, &dest_area);
    


}
