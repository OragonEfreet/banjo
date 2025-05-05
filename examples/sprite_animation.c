////////////////////////////////////////////////////////////////////////////////
/// \example sprite_animation.c
/// A quick example of animating a sprite.
///
/// This sprite animation is performed using \ref bj_bitmap_new_from_file to
/// load the sprite sheet in memory and \ref bj_bitmap_blit to display each
/// frame over time.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/window.h>

#define SPRITE_W 24
#define SPRITE_H 24
#define FRAMES 7

#define WINDOW_W 240
#define WINDOW_H 240

int main(int argc, char* argv[]) {


    bj_bitmap* bmp_rendering = bj_bitmap_new(SPRITE_W, SPRITE_H, BJ_PIXEL_MODE_BGR24, 0);
    bj_bitmap_set_clear_color(bmp_rendering, bj_bitmap_pixel_value(bmp_rendering, 0xFF, 0x00, 0x00));
    bj_bitmap_clear(bmp_rendering);

    bj_bitmap* bmp_sprite_sheet = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    bj_error* p_error = 0;

    if (!bj_system_init(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return 1;
    }

    bj_window* window     = bj_window_new("sprite sheet - Banjo", 0, 0, WINDOW_W, WINDOW_H, 0);
    bj_window_set_key_event(window, bj_close_on_escape);

    bj_bitmap* p_window_framebuffer = bj_window_get_framebuffer(window, 0);

    size_t frame_count = 1;

    while (!bj_window_should_close(window)) {
        bj_poll_events();

        bj_bitmap_blit(bmp_sprite_sheet, &(bj_rect){
            .x = frame_count * SPRITE_W,
                .w = 24, .h = 24
        }, bmp_rendering, & (bj_rect){.x = 0, .y = 0});

        bj_bitmap_blit_stretched(bmp_rendering, 0, p_window_framebuffer, 0);
        bj_window_update_framebuffer(window);

        bj_sleep(120);
        if (++frame_count >= FRAMES) {
            frame_count = 1;
        }
    }

    bj_system_dispose(0);

    bj_bitmap_del(bmp_sprite_sheet);
    bj_bitmap_del(bmp_rendering);

    return 0;
}


