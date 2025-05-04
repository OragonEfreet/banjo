////////////////////////////////////////////////////////////////////////////////
/// \example window.c
/// How to open and close windows.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/window.h>

void key_event(bj_window* p_window, bj_event_action action, bj_key key, int scancode) {
    (void)p_window;
    (void)scancode;

    if (key == BJ_KEY_ESCAPE && action == BJ_RELEASE) {
        bj_window_set_should_close(p_window);
    }
}

void draw(bj_bitmap* bmp) {
    bj_bitmap_clear(bmp);

    const uint32_t color_aquamarine = bj_bitmap_pixel_value(bmp, 0xFF, 0x00, 0xFF);
    const uint32_t color_cyan = bj_bitmap_pixel_value(bmp, 0x7F, 0xFF, 0xD4);
    const uint32_t color_white = bj_bitmap_pixel_value(bmp, 0xFF, 0xFF, 0xFF);

    // Draw pixels individually
    for (size_t x = 10; x < 490; ++x) {
        if (x % 7 == 0) {
            bj_bitmap_put_pixel(bmp, x, 10, color_aquamarine);
        }
    }

    // Draw lines (shapes a banjo)
    bj_pixel points[18] = {
        {100, 20},  {95,  25},  {95,  50},  {100, 55},  {100, 100}, {95,  100},
        {75,  120}, {75,  145}, {95,  165}, {120, 165}, {140, 145}, {140, 120},
        {120, 100}, {115, 100}, {115, 55},  {120, 50},  {120, 25},  {115, 20},
    };

    for (size_t p = 0; p < 18; ++p) {
        bj_bitmap_draw_line(bmp,
            points[p],
            points[(p + 1) % 18],
            color_cyan
        );
    }

    // Draw triangles (shapes a fox)
    bj_pixel verts[] = {
        {330, 270}, {270, 210}, {210, 270}, {210, 150}, {390, 210}, {450, 270},
        {450, 150}, {180, 330}, {270, 390}, {390, 390}, {480, 330}, {330, 450},
        {300, 480}, {360, 480},
    };

    size_t tris[13][3] = {
        {0, 1, 2},   {0, 2, 3},   {0, 4, 5},  {0, 1, 4},  {4, 6, 5}, {2, 8, 7},
        {0, 8, 2},   {0, 5, 9},   {9, 5, 10}, {8, 9, 11}, {8, 11, 12},
        {9, 13, 11}, {11, 12, 13},
    };


    for (size_t t = 0; t < 13; ++t) {
        bj_bitmap_draw_triangle(bmp,
            verts[tris[t][0]], verts[tris[t][1]], verts[tris[t][2]],
            color_white
        );
    }
}


int main(void) {

    bj_error* p_error = 0;

    if(!bj_system_init(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return 1;
    } 

    bj_window* window = bj_window_new("Simple Banjo Window", 100, 100, 500, 500, 0);

    bj_window_set_key_event(window, key_event);

    bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);

    draw(framebuffer);
    bj_window_update_framebuffer(window);

    while (!bj_window_should_close(window)) {
        bj_poll_events();
        bj_sleep(300);
    }

    bj_window_del(window);

    bj_system_dispose(0);
    return 0;
}
