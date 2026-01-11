////////////////////////////////////////////////////////////////////////////////
/// \example load_bmp.c
/// Loading and displaying BMP image files.
///
/// Banjo can load BMP files without external image libraries. This example
/// loads a sprite sheet, creates a window sized to the image, and displays
/// it scaled up for visibility.
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

bj_window* window               = 0;
bj_renderer* renderer           = 0;
bj_bitmap* bmp_sprite_sheet     = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    // Load a BMP file from disk. BANJO_ASSETS_DIR is a macro pointing to the
    // assets directory. The second parameter is an optional error pointer.
    // Returns NULL on failure (file not found, invalid format, etc.).
    bmp_sprite_sheet = bj_create_bitmap_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);

    // Create a window sized to the loaded bitmap. bj_bitmap_width() and
    // bj_bitmap_height() query the dimensions. We scale by 10 for visibility
    // since the sprite sheet is small.
    window = bj_bind_window("sprite sheet - Banjo", 0, 0,
        bj_bitmap_width(bmp_sprite_sheet) * 10,
        bj_bitmap_height(bmp_sprite_sheet) * 10,
        0
    );

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    // bj_blit_stretched() copies the source bitmap to the destination,
    // automatically scaling to fit. The NULL source/dest rects mean use the
    // entire bitmaps. BJ_BLIT_OP_COPY is a direct pixel copy.
    bj_blit_stretched(bmp_sprite_sheet, 0, bj_get_framebuffer(renderer), 0, BJ_BLIT_OP_COPY);
    bj_present(renderer, window);

    return bj_callback_continue;
}


int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();

    return bj_should_close_window(window)
         ? bj_callback_exit_success
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_end();

    // Always destroy loaded bitmaps to free memory. Do this after bj_end()
    // to ensure the bitmap isn't referenced by any Banjo subsystems.
    bj_destroy_bitmap(bmp_sprite_sheet);
    return status;
}


