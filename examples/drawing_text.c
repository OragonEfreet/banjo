////////////////////////////////////////////////////////////////////////////////
/// \example drawing_2d.c
///  2D primitive drawing demo.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window = 0;

void draw(bj_bitmap* bmp) {

    uint32_t white      = bj_bitmap_pixel_value(bmp,255,255,255);
    uint32_t black      = bj_bitmap_pixel_value(bmp,0,0,0);
    uint32_t light_grey = bj_bitmap_pixel_value(bmp,220,220,220);
    uint32_t cyan       = bj_bitmap_pixel_value(bmp,0,200,200);

    bj_bitmap_print(bmp, 20, 20,  18, white, "\x1B[31mRED\x1B[0m normal");
    bj_bitmap_print(bmp, 20, 52,  18, white, "\x1B[94mBrightBlue\x1B[0m + default");
    bj_bitmap_print(bmp, 20, 84,  18, white, "\x1B[38;2;255;128;0mTruecolor Orange\x1B[0m");
    bj_bitmap_print(bmp, 20, 200, 28, white, "Normal Text");

    // More control:
    bj_bitmap_blit_text(bmp, 20, 116, 14, black, light_grey, BJ_MASK_BG_OPAQUE, "OPAQUE band (FG black)");
    bj_bitmap_blit_text(bmp, 20, 150, 32, black, cyan, BJ_MASK_BG_REV_TRANSPARENT, "CARVED cyan");

}


int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Simple Text", 100, 100, 500, 500, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);
    draw(framebuffer);
    bj_window_update_framebuffer(window);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_sleep(300);

    return bj_window_should_close(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_window_del(window);
    bj_end(0);
    return status;
}

