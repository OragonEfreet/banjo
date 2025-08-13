////////////////////////////////////////////////////////////////////////////////
/// \example bitmap_blit.c
/// Demonstration on bitmap blit function.
///
/// Blitting a \ref bj_bitmap into another is done by calling \ref bj_bitmap_blit.
/// This example blits 3 bitmaps onto another and displays the result on screen.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define WINDOW_W 800
#define WINDOW_H 600

bj_window* window = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    bj_bitmap* bmp_rendering = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_BGR24, 0);
    bj_bitmap_set_clear_color(bmp_rendering, bj_bitmap_pixel_value(bmp_rendering, 0xFF, 0x00, 0x00));
    bj_bitmap_clear(bmp_rendering);

    bj_bitmap* bmp_blackbuck_512_512 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/blackbuck.bmp", 0);
    bj_bitmap* bmp_greenland_grid_velo_762_1309 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/greenland_grid_velo.bmp", 0);
    bj_bitmap* bmp_lena_512_512 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/lena.bmp", 0);
    bj_bitmap* bmp_snail_256_256 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/snail.bmp", 0);
    
    bj_bitmap_blit(bmp_greenland_grid_velo_762_1309, &(bj_rect){.x = 0, .y = 0, .w = 762, .h = 1309}, bmp_rendering, & (bj_rect){.x = 20, .y = 0}, BJ_BLIT_OP_COPY);
    bj_bitmap_blit(bmp_blackbuck_512_512, &(bj_rect){.x = 100, .y = 100, .w = 512, .h = 512}, bmp_rendering, & (bj_rect){.x = 100, .y = 200}, BJ_BLIT_OP_COPY);
    bj_bitmap_blit(bmp_snail_256_256, &(bj_rect){.x = 0, .y = 0, .w = 256, .h = 256}, bmp_rendering, & (bj_rect){.x = 500, .y = 130}, BJ_BLIT_OP_COPY);

    bj_bitmap_del(bmp_snail_256_256);
    bj_bitmap_del(bmp_lena_512_512);
    bj_bitmap_del(bmp_greenland_grid_velo_762_1309);
    bj_bitmap_del(bmp_blackbuck_512_512);

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Blitmap Blit", 0, 0, WINDOW_W, WINDOW_H, 0);
    bj_set_key_callback(bj_close_on_escape);

    bj_bitmap_blit(bmp_rendering, 0, bj_window_get_framebuffer(window, 0), 0, BJ_BLIT_OP_COPY);
    bj_bitmap_del(bmp_rendering);

    bj_window_update_framebuffer(window);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

    bj_sleep(30);

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



