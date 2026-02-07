////////////////////////////////////////////////////////////////////////////////
/// \example drawing_text.c
/// Text rendering with built-in fonts, ANSI colors, and printf-style formatting.
///
/// Banjo includes bitmap text rendering with a built-in font. No external font
/// files are needed. Text can be rendered with ANSI escape codes for inline
/// colors, printf-style formatting, and various transparency modes.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window = 0;
bj_renderer* renderer = 0;

void draw(bj_bitmap* bmp) {

    uint32_t white      = bj_make_bitmap_pixel(bmp,255,255,255);
    uint32_t black      = bj_make_bitmap_pixel(bmp,0,0,0);
    uint32_t light_grey = bj_make_bitmap_pixel(bmp,220,220,220);
    uint32_t cyan       = bj_make_bitmap_pixel(bmp,0,200,200);

    // bj_draw_text() renders text with ANSI escape code support for inline
    // colors. Use \x1B[<code>m for colors and \x1B[0m to reset to default.
    // Parameters: bitmap, x, y, size, foreground_color, text
    bj_draw_text(bmp, 20, 20,  18, white, "\x1B[31mRED\x1B[0m normal");
    bj_draw_text(bmp, 20, 52,  18, white, "\x1B[94mBrightBlue\x1B[0m + default");
    bj_draw_text(bmp, 20, 84,  18, white, "\x1B[38;2;255;128;0mTruecolor Orange\x1B[0m");

    // bj_draw_textf() works like printf, supporting all standard format
    // specifiers. This is the easiest way to display dynamic values like
    // scores, health, coordinates, etc.
    bj_draw_textf(bmp, 20, 200, 18, white, "Hello, %s!", "world");
    bj_draw_textf(bmp, 20, 230, 18, white, "Score: %d  Lives: %u", -123, 3);
    bj_draw_textf(bmp, 20, 260, 18, white, "Zero-pad: %08u  Left: %-8u|", 42, 42);
    bj_draw_textf(bmp, 20, 290, 18, white, "Name: %.5s  Pi≈%.3f (note: if %%f not supported, skip)", "Banjo", 3.14159);
    bj_draw_textf(bmp, 20, 320, 18, white, "HEX: 0x%08X  oct: %o  ptr: %p", 0xDEADBEEF, 0755, (void*)bmp);
    bj_draw_textf(bmp, 20, 350, 18, white, "Width(*)=%*u  Prec(*)=%. *u", 6, 123, 4, 123);

    // Length modifiers (ll, l, h, hh) are supported for different integer sizes.
    unsigned long long big = 18446744073709551615ull;
    bj_draw_textf(bmp, 20, 380, 18, white, "ll: %llu  l: %ld  h: %hd  hh: %hhu",
                     big, (long)-123456, (short)1234, (unsigned char)255);

    // bj_blit_text() provides more control with separate foreground and
    // background colors and transparency modes:
    //   BJ_MASK_BG_OPAQUE - solid background rectangle
    //   BJ_MASK_BG_REV_TRANSPARENT - background shows through foreground
    bj_blit_text(bmp, 20, 116, 14, black, light_grey, BJ_MASK_BG_OPAQUE, "OPAQUE band (FG black)");
    bj_blit_text(bmp, 20, 150, 32, black, cyan, BJ_MASK_BG_REV_TRANSPARENT, "CARVED cyan");
}


int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    } 

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE, 0);
    window = bj_bind_window("Simple Text", 100, 100, 500, 500, 0, 0);

    bj_renderer_configure(renderer, window, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    draw(bj_get_framebuffer(renderer));
    bj_present(renderer, window);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_sleep(300);

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_end();
    return status;
}

