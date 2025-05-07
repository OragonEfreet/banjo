////////////////////////////////////////////////////////////////////////////////
/// \example bitmap_blit.c
/// Demonstration on bitmap blit function.
///
/// Blitting a \ref bj_bitmap into another is done by calling \ref bj_bitmap_blit.
/// This example blits 3 bitmaps onto another and displays the result on screen.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/window.h>

#define WINDOW_W 800
#define WINDOW_H 600

int main() {

    bj_bitmap* bmp_rendering = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_BGR24, 0);
    bj_bitmap_set_clear_color(bmp_rendering, bj_bitmap_pixel_value(bmp_rendering, 0xFF, 0x00, 0x00));
    bj_bitmap_clear(bmp_rendering);

    bj_bitmap* bmp_blackbuck_512_512 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/blackbuck.bmp", 0);
    bj_bitmap* bmp_greenland_grid_velo_762_1309 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/greenland_grid_velo.bmp", 0);
    bj_bitmap* bmp_lena_512_512 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/lena.bmp", 0);
    bj_bitmap* bmp_snail_256_256 = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/snail.bmp", 0);

    bj_bitmap_blit(bmp_greenland_grid_velo_762_1309, &(bj_rect){.x = 0, .y = 0, .w = 762, .h = 1309}, bmp_rendering, & (bj_rect){.x = 20, .y = 0});
    bj_bitmap_blit(bmp_blackbuck_512_512, &(bj_rect){.x = 100, .y = 100, .w = 512, .h = 512}, bmp_rendering, & (bj_rect){.x = 100, .y = 200});
    bj_bitmap_blit(bmp_snail_256_256, &(bj_rect){.x = 0, .y = 0, .w = 256, .h = 256}, bmp_rendering, & (bj_rect){.x = 500, .y = 130});

    bj_bitmap_del(bmp_snail_256_256);
    bj_bitmap_del(bmp_lena_512_512);
    bj_bitmap_del(bmp_greenland_grid_velo_762_1309);
    bj_bitmap_del(bmp_blackbuck_512_512);


    if (bj_system_init(0)) {

        bj_window* window = bj_window_new("Blitmap Blit", 0, 0, WINDOW_W, WINDOW_H, 0);
        bj_window_set_key_event(window, bj_close_on_escape);

        bj_bitmap_blit(bmp_rendering, 0, bj_window_get_framebuffer(window, 0), 0);
        bj_window_update_framebuffer(window);

        while (!bj_window_should_close(window)) {
            bj_poll_events();
            bj_sleep(30);
        }
        
        bj_window_del(window);
        bj_system_dispose(0);
    }

    bj_bitmap_del(bmp_rendering);

    return 0;
}


