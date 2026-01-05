// Note: This file is AI-generated.

#include <stdint.h>
#include <stddef.h>

#include <banjo/log.h>
#include <banjo/math.h>
#include <banjo/string.h>

#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/bitmap.h>

#include <bitmap_charsets.h>
#include <bitmap.h>
#include <check.h>

#include <stdio.h>
#include <stdarg.h>

#define CHARSET_CHAR_PER_ROW 16
#define CHAR_PIXEL_X(c) ((int)((c) % CHARSET_CHAR_PER_ROW) * CHAR_PIXEL_W)
#define CHAR_PIXEL_Y(c) ((int)((c) / CHARSET_CHAR_PER_ROW) * CHAR_PIXEL_H)

/* =========================
   8bpp glyph mask atlas
   ========================= */
static const struct bj_bitmap* get_charset_mask(struct bj_bitmap* p_bitmap)
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

    struct bj_bitmap* mask = bj_create_bitmap(
        charset_pixel_width, charset_pixel_height,
        BJ_PIXEL_MODE_INDEXED_8, 0
    );
    bj_check_or_0(mask);

    bj_set_bitmap_clear_color(mask, 0x00u);
    bj_clear_bitmap(mask);

    for (size_t idx = 0; idx < charset_char_len; ++idx) {
        const int base_x = CHAR_PIXEL_X((int)idx);
        const int base_y = CHAR_PIXEL_Y((int)idx);

        for (int row = 0; row < CHAR_PIXEL_H; ++row) {
            const uint8_t rowbits = charset_latin1[idx][row];
            for (int col = 0; col < CHAR_PIXEL_W; ++col) {
                if ((rowbits >> col) & 0x01u) {
                    bj_put_pixel(
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
static void fast_fill_rect(struct bj_bitmap* dst, const struct bj_rect* r, uint32_t color_native)
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
                bj_put_pixel(dst, (size_t)x, (size_t)y, color_native);
            }
        }
        return;
    }

    const size_t bytes_per_px = bpp >> 3;

    for (int y = y0; y < y1; ++y) {
        /* Seed first pixel via API to ensure correct native packing. */
        bj_put_pixel(dst, (size_t)x0, (size_t)y, color_native);

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


/* ---------- ANSI color helpers ---------- */

static void ansi_basic_rgb(uint8_t idx, uint8_t* r, uint8_t* g, uint8_t* b, bj_bool bright)
{
    /* idx = 0..7 for black,red,green,yellow,blue,magenta,cyan,white */
    static const uint8_t base[8][3] = {
        {0,0,0}, {205,0,0}, {0,205,0}, {205,205,0},
        {0,0,238}, {205,0,205}, {0,205,205}, {229,229,229}
    };
    static const uint8_t hi[8][3] = {
        {127,127,127}, {255,0,0}, {0,255,0}, {255,255,0},
        {92,92,255}, {255,0,255}, {0,255,255}, {255,255,255}
    };
    const uint8_t (*tbl)[3] = bright ? hi : base;
    *r = tbl[idx][0]; *g = tbl[idx][1]; *b = tbl[idx][2];
}

static uint32_t pack_native_rgb(const struct bj_bitmap* dst, uint8_t r, uint8_t g, uint8_t b)
{
    return bj_make_bitmap_pixel((struct bj_bitmap*)dst, r, g, b);
}

/* Parse a sequence starting at text[i] which is after the ESC '['.
   Fills new_fg/new_bg if set; returns new index positioned at the final 'm'
   (or the last parsed char), and a flag telling if we consumed a valid SGR. */
static size_t parse_ansi_sgr(
    const char* text, size_t i, size_t len,
    const struct bj_bitmap* dst,
    uint32_t default_fg, uint32_t default_bg,
    bj_bool* out_changed,
    uint32_t* new_fg, uint32_t* new_bg
){
    *out_changed = BJ_FALSE;

    /* Collect up to, say, 16 parameters: "p1;p2;...;pn m" */
    int params[16];
    int nparams = 0;
    int cur = 0;
    bj_bool have_cur = BJ_FALSE;

    while (i < len) {
        char ch = text[i++];
        if (ch >= '0' && ch <= '9') {
            cur = cur * 10 + (ch - '0');
            have_cur = BJ_TRUE;
            continue;
        }
        if (ch == ';') {
            if (have_cur && nparams < 16) params[nparams++] = cur;
            cur = 0; have_cur = BJ_FALSE;
            continue;
        }
        if (ch == 'm') {
            if (have_cur && nparams < 16) params[nparams++] = cur;
            break; /* end of SGR */
        }
        /* Unexpected char: abort */
        return i;
    }

    if (nparams == 0) {
        /* ESC[m] == reset */
        *new_fg = default_fg; *new_bg = default_bg;
        *out_changed = BJ_TRUE;
        return i;
    }

    /* Process the params left-to-right; we support:
       0        reset
       30-37    fg basic
       40-47    bg basic
       90-97    fg bright
       100-107  bg bright
       39       default fg
       49       default bg
       38;2;r;g;b  truecolor fg
       48;2;r;g;b  truecolor bg
    */
    uint32_t tmp_fg = *new_fg;
    uint32_t tmp_bg = *new_bg;

    for (int p = 0; p < nparams; ++p) {
        int code = params[p];

        if (code == 0) { /* reset colors */
            tmp_fg = default_fg; tmp_bg = default_bg;
            continue;
        }
        if (code == 39) { tmp_fg = default_fg; continue; }
        if (code == 49) { tmp_bg = default_bg; continue; }

        if (code == 38 || code == 48) {
            /* Expect ;2;r;g;b */
            if (p + 4 < nparams && params[p+1] == 2) {
                int r = params[p+2], g = params[p+3], b = params[p+4];
                if (r < 0)   r = 0;
                if (r > 255) r = 255;
                if (g < 0)   g = 0;
                if (g > 255) g = 255;
                if (b < 0)   b = 0;
                if (b > 255) b = 255;
                uint32_t v = pack_native_rgb(dst, (uint8_t)r, (uint8_t)g, (uint8_t)b);
                if (code == 38) tmp_fg = v; else tmp_bg = v;
                p += 4;
                continue;
            }
            /* Unsupported 256-color form 38;5;n / 48;5;n → ignore gracefully */
            continue;
        }

        if ((code >= 30 && code <= 37) || (code >= 90 && code <= 97)) {
            uint8_t idx = (uint8_t)((code >= 90) ? (code - 90) : (code - 30));
            uint8_t r,g,b; ansi_basic_rgb(idx, &r,&g,&b, code >= 90);
            tmp_fg = pack_native_rgb(dst, r,g,b);
            continue;
        }
        if ((code >= 40 && code <= 47) || (code >= 100 && code <= 107)) {
            uint8_t idx = (uint8_t)((code >= 100) ? (code - 100) : (code - 40));
            uint8_t r,g,b; ansi_basic_rgb(idx, &r,&g,&b, code >= 100);
            tmp_bg = pack_native_rgb(dst, r,g,b);
            continue;
        }
        /* Unknown code → ignore */
    }

    *new_fg = tmp_fg; *new_bg = tmp_bg;
    *out_changed = BJ_TRUE;
    return i;
}


/* =========================
   Text renderer
   ========================= */
static void render_text_masked(
    struct bj_bitmap*      dst,
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

    /* Atlas (8bpp, 0/255) */
    const struct bj_bitmap* mask = get_charset_mask(dst);
    bj_check(mask);

    /* Target glyph box (keeps aspect from CHAR_PIXEL_W×CHAR_PIXEL_H) */
    const uint16_t glyph_w = (uint16_t)((height * CHAR_PIXEL_W + CHAR_PIXEL_H/2) / CHAR_PIXEL_H);
    const uint16_t glyph_h = (uint16_t)height;

    /* Spacing between glyph boxes in pixels */
    int spacing = (int)bj_round(0.1 * (double)CHAR_PIXEL_W);
    if (spacing < 1) spacing = 1;

    const size_t table_len = sizeof charset_latin1 / sizeof charset_latin1[0];
    const size_t len = bj_strlen(text);

    const int dst_w = (int)dst->width;
    const int dst_h = (int)dst->height;

    /* Current colors (can be changed by ANSI sequences) */
    uint32_t fg = fg_native;
    uint32_t bg = bg_native;

    /* Track pen x in int to avoid int16_t overflow */
    int pen_x = x;
    const int pen_y = y;

    size_t i = 0;
    while (i < len) {
        unsigned char ch = (unsigned char)text[i];

        /* ANSI escape: ESC '[' ... 'm' */
        if (ch == 0x1B && i + 1 < len && text[i+1] == '[') {
            i += 2; /* skip ESC [ */
            bj_bool changed = BJ_FALSE;
            uint32_t new_fg = fg, new_bg = bg;
            i = parse_ansi_sgr(text, i, len, dst, fg_native, bg_native, &changed, &new_fg, &new_bg);
            if (i < len && text[i] == 'm') ++i; /* consume trailing 'm' if present */
            if (changed) { fg = new_fg; bg = new_bg; }
            continue;
        }

        /* Stop if we are completely to the right of the surface */
        if (pen_x >= dst_w) break;

        /* Map to charset index (fallback to '?') */
        uint8_t code = (uint8_t)((ch < table_len) ? ch : (unsigned char)'?');

        /* Full source glyph (in atlas space) */
        struct bj_rect src_full = {
            .x = (int16_t)CHAR_PIXEL_X((int)code),
            .y = (int16_t)CHAR_PIXEL_Y((int)code),
            .w = (uint16_t)CHAR_PIXEL_W,
            .h = (uint16_t)CHAR_PIXEL_H
        };

        /* Desired destination box before clipping */
        struct bj_rect dst_box = {
            .x = (int16_t)pen_x,
            .y = (int16_t)pen_y,
            .w = glyph_w,
            .h = glyph_h
        };

        /* Early out if the entire glyph box starts below or to the right */
        if (dst_box.x >= dst_w || dst_box.y >= dst_h) break;
        if ((int)dst_box.x + (int)dst_box.w <= 0 || (int)dst_box.y + (int)dst_box.h <= 0) {
            /* Entirely left or above; just advance pen */
            pen_x += (int)glyph_w + spacing;
            ++i;
            continue;
        }

        /* Clip dst_box against surface, and proportionally adjust src_full.
           We do left/top/right/bottom separately. */

        struct bj_rect src_adj = src_full;

        /* --- Horizontal clip --- */
        /* Left clip */
        if (dst_box.x < 0) {
            int clip = -dst_box.x;
            if (clip >= (int)dst_box.w) { /* fully clipped horizontally */
                pen_x += (int)glyph_w + spacing;
                ++i;
                continue;
            }
            /* Proportional shift in source */
            uint16_t src_shift = (uint16_t)(((uint32_t)clip * CHAR_PIXEL_W) / glyph_w);
            if (src_shift >= src_adj.w) src_shift = src_adj.w - 1;
            src_adj.x = (int16_t)(src_adj.x + (int)src_shift);
            src_adj.w = (uint16_t)(src_adj.w - src_shift);

            dst_box.x = 0;
            dst_box.w = (uint16_t)((int)dst_box.w - clip);
        }

        /* Right clip */
        {
            int over = (int)dst_box.x + (int)dst_box.w - dst_w;
            if (over > 0) {
                if (over >= (int)dst_box.w) { /* fully clipped horizontally */
                    pen_x += (int)glyph_w + spacing;
                    ++i;
                    continue;
                }
                uint16_t keep = (uint16_t)((int)dst_box.w - over);
                /* Proportional shrink of source width */
                uint16_t src_keep = (uint16_t)(((uint32_t)keep * CHAR_PIXEL_W) / glyph_w);
                if (src_keep == 0) src_keep = 1;
                if (src_keep > src_adj.w) src_keep = src_adj.w;
                src_adj.w = src_keep;
                dst_box.w = keep;
            }
        }

        /* --- Vertical clip --- */
        /* Top clip */
        if (dst_box.y < 0) {
            int clip = -dst_box.y;
            if (clip >= (int)dst_box.h) { /* fully clipped vertically */
                pen_x += (int)glyph_w + spacing;
                ++i;
                continue;
            }
            uint16_t src_shift = (uint16_t)(((uint32_t)clip * CHAR_PIXEL_H) / glyph_h);
            if (src_shift >= src_adj.h) src_shift = src_adj.h - 1;
            src_adj.y = (int16_t)(src_adj.y + (int)src_shift);
            src_adj.h = (uint16_t)(src_adj.h - src_shift);

            dst_box.y = 0;
            dst_box.h = (uint16_t)((int)dst_box.h - clip);
        }

        /* Bottom clip */
        {
            int over = (int)dst_box.y + (int)dst_box.h - dst_h;
            if (over > 0) {
                if (over >= (int)dst_box.h) { /* fully clipped vertically */
                    pen_x += (int)glyph_w + spacing;
                    ++i;
                    continue;
                }
                uint16_t keep = (uint16_t)((int)dst_box.h - over);
                uint16_t src_keep = (uint16_t)(((uint32_t)keep * CHAR_PIXEL_H) / glyph_h);
                if (src_keep == 0) src_keep = 1;
                if (src_keep > src_adj.h) src_keep = src_adj.h;
                src_adj.h = src_keep;
                dst_box.h = keep;
            }
        }

        /* If anything remains, blit the (possibly clipped) glyph */
        if (dst_box.w > 0 && dst_box.h > 0 && src_adj.w > 0 && src_adj.h > 0) {
            bj_blit_mask_stretched(
                mask, &src_adj,
                dst, &dst_box,
                fg, bg, mode
            );

            /* Fill spacing gap (carved mode), clipped to right edge */
            if (mode == BJ_MASK_BG_REV_TRANSPARENT && spacing > 0) {
                int gap_x = (int)dst_box.x + (int)dst_box.w;
                if (gap_x < dst_w) {
                    uint16_t gap_w = (uint16_t)((gap_x + spacing <= dst_w) ? spacing : (dst_w - gap_x));
                    if (gap_w) {
                        struct bj_rect gap = { (int16_t)gap_x, (int16_t)dst_box.y, gap_w, dst_box.h };
                        fast_fill_rect(dst, &gap, bg);
                    }
                }
            }
        }

        /* Advance pen by the *intended* glyph width (not the clipped width) plus spacing.
           This preserves fixed advance even when the last glyph is partially clipped. */
        pen_x += (int)glyph_w + spacing;
        ++i;
    }
}

/* Public: FG+BG with selectable background mode */
void bj_blit_text(
    struct bj_bitmap*      dst,
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

void bj_draw_text(
    struct bj_bitmap*      dst,
    int             x,
    int             y,
    unsigned        height,
    uint32_t        fg_native,
    const char*     text
) {
    render_text_masked(dst, x, y, height, fg_native, 0, BJ_MASK_BG_TRANSPARENT, text);
}

void bj_draw_vtextf(
    struct bj_bitmap*     p_bitmap,
    int            x,
    int            y,
    unsigned       height,
    uint32_t       fg_native,
    const char*    fmt,
    va_list        args
) {
    bj_check(p_bitmap);
    bj_check(fmt);
    bj_check(height > 0);

    /* First pass: measure required length (excluding NUL) */
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (needed < 0) {
        /* Formatting failed; nothing to draw */
        return;
    }

    /* Allocate exact buffer (+1 for NUL). Fallback to small stack if alloc fails. */
    char* dyn = (char*)bj_malloc((size_t)needed + 1u);
    if (dyn) {
        int written = vsnprintf(dyn, (size_t)needed + 1u, fmt, args);
        if (written >= 0) {
            bj_draw_text(p_bitmap, x, y, height, fg_native, dyn);
        }
        bj_free(dyn);
        return;
    }

    /* Fallback: truncated stack buffer (no heap). */
    {
        char small[1024];
        int written = vsnprintf(small, sizeof small, fmt, args);
        if (written >= 0) {
            small[sizeof small - 1] = '\0';
            bj_draw_text(p_bitmap, x, y, height, fg_native, small);
        }
    }
}

void bj_draw_textf(
    struct bj_bitmap*     p_bitmap,
    int            x,
    int            y,
    unsigned       height,
    uint32_t       fg_native,
    const char*    fmt,
    ...
) {
    va_list args;
    va_start(args, fmt);
    bj_draw_vtextf(p_bitmap, x, y, height, fg_native, fmt, args);
    va_end(args);
}
