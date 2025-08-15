////////////////////////////////////////////////////////////////////////////////
/// \example drawing_2d.c
///  2D primitive drawing demo.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window = 0;

void draw(bj_bitmap* bmp) {
    bj_bitmap_clear(bmp);

    const uint32_t color_red = bj_bitmap_pixel_value(bmp, 0xFF, 0x00, 0x00);
    const uint32_t color_cyan = bj_bitmap_pixel_value(bmp, 0x7F, 0xFF, 0xD4);
    const uint32_t color_white = bj_bitmap_pixel_value(bmp, 0xFF, 0xFF, 0xFF);

    // Draw pixels individually
    for (size_t x = 10; x < 490; ++x) {
        if (x % 7 == 0) {
            bj_bitmap_put_pixel(bmp, x, 10, color_red);
        }
    }

    // Draw lines (shapes a banjo)
    int points[][2] = {
        {100, 20},  {95,  25},  {95,  50},  {100, 55},  {100, 100}, {95,  100},
        {75,  120}, {75,  145}, {95,  165}, {120, 165}, {140, 145}, {140, 120},
        {120, 100}, {115, 100}, {115, 55},  {120, 50},  {120, 25},  {115, 20},
    };

    for (size_t p = 0; p < 18; ++p) {
        bj_bitmap_draw_line(bmp,
            points[p][0],
            points[p][1],
            points[(p + 1) % 18][0],
            points[(p + 1) % 18][1],
            color_cyan
        );
    }

    // Draw triangles (shapes a fox)
    int verts[][2] = {
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
            verts[tris[t][0]][0], verts[tris[t][0]][1], 
            verts[tris[t][1]][0], verts[tris[t][1]][1], 
            verts[tris[t][2]][0], verts[tris[t][2]][1],
            color_white
        );
    }

    //Draw a checker board
    bj_rect board = {.x = 100, .y = 100, .w = 80, .h = 80,};
    bj_bitmap_draw_rectangle(bmp,
        &board,
        color_red
    );
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Simple Text", 100, 100, 500, 500, 0);
    bj_set_key_callback(bj_close_on_escape);

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

