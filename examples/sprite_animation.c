////////////////////////////////////////////////////////////////////////////////
/// \example sprite_animation.c
/// Frame-based sprite animation using sprite sheets.
///
/// Sprite animation works by loading a sprite sheet (multiple frames arranged
/// in a grid) and blitting one frame at a time. By cycling through frames over
/// time, the sprite appears to animate. This example demonstrates the core
/// technique: use a frame counter to calculate which horizontal slice of the
/// sprite sheet to blit each iteration.
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

// This sprite sheet has 7 frames arranged horizontally, each 24x24 pixels.
// Frame layout: [0][1][2][3][4][5][6]
#define SPRITE_W 24
#define SPRITE_H 24
#define FRAMES 7

#define WINDOW_W 240
#define WINDOW_H 240

bj_window* window               = 0;
bj_renderer* renderer           = 0;
bj_bitmap* p_window_framebuffer = 0;
// Frame counter determines which frame to display (1-7).
size_t     frame_count          = 1;
bj_bitmap* bmp_rendering        = 0;
bj_bitmap* bmp_sprite_sheet     = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    // Create a small off-screen bitmap to hold one animation frame.
    bmp_rendering = bj_create_bitmap(SPRITE_W, SPRITE_H, BJ_PIXEL_MODE_BGR24, 0);
    bj_set_bitmap_color(bmp_rendering, bj_make_bitmap_pixel(bmp_rendering, 0xFF, 0x00, 0x00), BJ_BITMAP_CLEAR_COLOR);
    bj_clear_bitmap(bmp_rendering);

    // Load the sprite sheet once. It stays in memory - we just blit different
    // regions from it each frame to create animation.
    bmp_sprite_sheet = bj_create_bitmap_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE, 0);
    window = bj_bind_window("sprite sheet - Banjo", 0, 0, WINDOW_W, WINDOW_H, 0, 0);

    bj_renderer_configure(renderer, window, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    // Cache the framebuffer pointer for faster access during animation.
    p_window_framebuffer = bj_get_framebuffer(renderer);

    return bj_callback_continue;
}


int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();

    // The animation technique: blit ONE frame from the sprite sheet by
    // calculating the horizontal offset. Frame 1 starts at x=24, frame 2 at
    // x=48, etc. This shifts which 24x24 region gets copied from the sheet.
    bj_blit(bmp_sprite_sheet, &(bj_rect){
            .x = frame_count * SPRITE_W,  // Horizontal offset into sprite sheet
            .w = 24, .h = 24              // Size of one frame
        }, bmp_rendering, & (bj_rect){.x = 0, .y = 0},
        BJ_BLIT_OP_COPY
    );

    // Scale the small 24x24 frame to fill the 240x240 window for visibility.
    bj_blit_stretched(bmp_rendering, 0, p_window_framebuffer, 0, BJ_BLIT_OP_COPY);
    bj_present(renderer, window);

    // Control animation speed. 120ms per frame ≈ 8 FPS.
    bj_sleep(120);
    // Advance to next frame and wrap around (1→2→...→7→1).
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

    // Destroy the sprite sheet and off-screen buffer.
    bj_destroy_bitmap(bmp_sprite_sheet);
    bj_destroy_bitmap(bmp_rendering);
    return status;
}


