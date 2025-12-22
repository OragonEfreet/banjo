////////////////////////////////////////////////////////////////////////////////
/// \example bitmap_blit_colorkey.c
/// Demonstration of color key transparency for sprite blitting.
///
/// This example shows how to use \ref bj_set_bitmap_colorkey to make specific
/// pixel values transparent during blitting. This is commonly used for sprite
/// rendering where a background color (like black or magenta) should be treated
/// as transparent. The example uses the Gabe sprite sheet with black (RGB: 0, 0, 0)
/// as the color key.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/memory.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define WINDOW_W 800
#define WINDOW_H 600

bj_window* window     = 0;
bj_renderer* renderer = 0;

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    // Create rendering target with a gradient background
    bj_bitmap* bmp_rendering = bj_create_bitmap(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_BGR24, 0);

    // Draw a checkerboard pattern to show transparency
    for (int y = 0; y < WINDOW_H; y += 40) {
        for (int x = 0; x < WINDOW_W; x += 40) {
            uint32_t color = ((x/40 + y/40) % 2 == 0)
                ? bj_make_bitmap_pixel(bmp_rendering, 0xD0, 0xD0, 0xD0)
                : bj_make_bitmap_pixel(bmp_rendering, 0xA0, 0xA0, 0xA0);

            bj_draw_filled_rectangle(
                bmp_rendering,
                &(bj_rect){.x = x, .y = y, .w = 40, .h = 40},
                color
            );
        }
    }

    // Load sprite sheet
    bj_bitmap* sprite_sheet = bj_create_bitmap_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    // The sprite uses black (RGB: 0, 0, 0) as the transparent color
    // We need to convert this to the native format of the sprite bitmap
    uint32_t black_key = bj_make_bitmap_pixel(sprite_sheet, 0x00, 0x00, 0x00);

    // Enable color key transparency on the sprite
    bj_set_bitmap_colorkey(sprite_sheet, BJ_TRUE, black_key);

    // Blit multiple sprites to demonstrate transparency
    // Each sprite frame is 24x24 pixels in the sprite sheet

    // Row 1: Idle animation frames
    for (int i = 0; i < 4; i++) {
        bj_blit(
            sprite_sheet,
            &(bj_rect){.x = i * 24, .y = 0, .w = 24, .h = 24},
            bmp_rendering,
            &(bj_rect){.x = 100 + i * 60, .y = 100},
            BJ_BLIT_OP_COPY
        );
    }

    // Row 2: Run animation frames
    for (int i = 0; i < 6; i++) {
        bj_blit(
            sprite_sheet,
            &(bj_rect){.x = i * 24, .y = 24, .w = 24, .h = 24},
            bmp_rendering,
            &(bj_rect){.x = 50 + i * 60, .y = 200},
            BJ_BLIT_OP_COPY
        );
    }

    // Draw some sprites at larger scale using bj_blit_stretched
    bj_blit_stretched(
        sprite_sheet,
        &(bj_rect){.x = 0, .y = 0, .w = 24, .h = 24},
        bmp_rendering,
        &(bj_rect){.x = 250, .y = 350, .w = 96, .h = 96},
        BJ_BLIT_OP_COPY
    );

    bj_blit_stretched(
        sprite_sheet,
        &(bj_rect){.x = 24, .y = 24, .w = 24, .h = 24},
        bmp_rendering,
        &(bj_rect){.x = 450, .y = 350, .w = 96, .h = 96},
        BJ_BLIT_OP_COPY
    );

    bj_destroy_bitmap(sprite_sheet);

    if(!bj_initialize(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("Color Key Transparency Demo", 0, 0, WINDOW_W, WINDOW_H, 0);

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    bj_blit(bmp_rendering, 0, bj_get_framebuffer(renderer), 0, BJ_BLIT_OP_COPY);
    bj_destroy_bitmap(bmp_rendering);

    bj_present(renderer, window);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

    bj_sleep(30);

    return bj_should_close_window(window)
         ? bj_callback_exit_success
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_shutdown(0);
    return status;
}
