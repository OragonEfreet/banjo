#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/error.h>
#include <banjo/main.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/window.h>
#include <banjo/log.h>

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    if (!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return 1;
    }

    bj_renderer* renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    bj_window* window = bj_bind_window("My First Banjo App", 100, 100, 640, 480, 0);
    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    // Draw some shapes
    bj_bitmap* bmp = bj_get_framebuffer(renderer);
    bj_clear_bitmap(bmp);

    const uint32_t red = bj_make_bitmap_pixel(bmp, 0xFF, 0x00, 0x00);
    const uint32_t cyan = bj_make_bitmap_pixel(bmp, 0x00, 0xFF, 0xFF);

    bj_draw_filled_circle(bmp, 320, 240, 100, red);
    bj_draw_rectangle(bmp, &(bj_rect){.x = 200, .y = 120, .w = 240, .h = 240}, cyan);

    bj_present(renderer, window);

    // Main loop
    while (!bj_should_close_window(window)) {
        bj_dispatch_events();
    }

    // Cleanup
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_end();
    return 0;
}
