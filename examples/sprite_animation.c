////////////////////////////////////////////////////////////////////////////////
/// \example sprite_animation.c
/// A quick example of animating a sprite.
///
/// This sprite animation is performed using \ref bj_create_bitmap_from_file to
/// load the sprite sheet in memory and \ref bj_blit to display each
/// frame over time.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/memory.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define SPRITE_W 24
#define SPRITE_H 24
#define FRAMES 7

#define WINDOW_W 240
#define WINDOW_H 240

bj_window* window               = 0;
bj_renderer* renderer           = 0;
bj_bitmap* p_window_framebuffer = 0;
size_t     frame_count          = 1;
bj_bitmap* bmp_rendering        = 0;
bj_bitmap* bmp_sprite_sheet     = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bmp_rendering = bj_create_bitmap(SPRITE_W, SPRITE_H, BJ_PIXEL_MODE_BGR24, 0);
    bj_set_bitmap_clear_color(bmp_rendering, bj_make_bitmap_pixel(bmp_rendering, 0xFF, 0x00, 0x00));
    bj_clear_bitmap(bmp_rendering);

    bmp_sprite_sheet = bj_create_bitmap_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    } 

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("sprite sheet - Banjo", 0, 0, WINDOW_W, WINDOW_H, 0);

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    p_window_framebuffer = bj_get_framebuffer(renderer);

    return bj_callback_continue;
}


int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();

    bj_blit(bmp_sprite_sheet, &(bj_rect){
            .x = frame_count * SPRITE_W,
            .w = 24, .h = 24
        }, bmp_rendering, & (bj_rect){.x = 0, .y = 0},
        BJ_BLIT_OP_COPY
    );

    bj_blit_stretched(bmp_rendering, 0, p_window_framebuffer, 0, BJ_BLIT_OP_COPY);
    bj_present(renderer, window);

    bj_sleep(120);
    if (++frame_count >= FRAMES) {
        frame_count = 1;
    }

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_unbind_window(window);
    bj_destroy_renderer(renderer);
    bj_end();

    bj_destroy_bitmap(bmp_sprite_sheet);
    bj_destroy_bitmap(bmp_rendering);
    return status;
}


