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
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

bj_window* window     = 0;
bj_renderer* renderer = 0;

void draw(bj_bitmap* bmp) {
    bj_clear_bitmap(bmp);

    const uint32_t color_red = bj_make_bitmap_pixel(bmp, 0xFF, 0x00, 0x00);
    const uint32_t color_cyan = bj_make_bitmap_pixel(bmp, 0x7F, 0xFF, 0xD4);
    const uint32_t color_white = bj_make_bitmap_pixel(bmp, 0xFF, 0xFF, 0xFF);

    // Draw pixels individually
    for (size_t x = 10; x < 490; ++x) {
        if (x % 7 == 0) {
            bj_put_pixel(bmp, x, 10, color_red);
        }
    }

    // Draw lines (shapes a banjo)
    int poly_x[] = { 100, 95, 95, 100, 100, 95,  75,  75,  95,  120, 140, 140, 120, 115, 115, 120, 120, 115, };
    int poly_y[] = { 20,  25, 50, 55,  100, 100, 120, 145, 165, 165, 145, 120, 100, 100, 55,  50,  25,  20, };
    bj_draw_polyline(bmp, 18, poly_x, poly_y, BJ_TRUE, color_cyan);

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
        bj_draw_triangle(bmp,
            verts[tris[t][0]][0], verts[tris[t][0]][1], 
            verts[tris[t][1]][0], verts[tris[t][1]][1], 
            verts[tris[t][2]][0], verts[tris[t][2]][1],
            color_white
        );
    }

    //Draw a checker board
    bj_rect board = {.w = 10, .h = 10,};
    for(size_t y = 0 ; y < 8 ; ++y) {
        for(size_t x = 0 ; x < 8 ; ++x) {
            board.x = 200 + x * board.w;
            board.y = 50 + y * board.h;
            if((x ^ y) & 1) {
                bj_draw_filled_rectangle(bmp,
                    &board,
                    color_red
                );
            } 
        }
    }
    bj_draw_rectangle(bmp,
        &(bj_rect) {.x = 200, .y = 50, .w = 80, .h = 80,},
        color_cyan
    );


    // Circle
    for (int r = 80; r > 0; r -= 20) {
        bj_draw_filled_circle(bmp, 100, 400, r, (r/20) % 2 ? color_red : color_white);
    }

    // Filled triangles (mountain scene)
    const uint32_t color_dark_gray = bj_make_bitmap_pixel(bmp, 0x50, 0x50, 0x50);
    const uint32_t color_gray = bj_make_bitmap_pixel(bmp, 0x80, 0x80, 0x80);
    const uint32_t color_light_gray = bj_make_bitmap_pixel(bmp, 0xB0, 0xB0, 0xB0);

    bj_draw_filled_triangle(bmp, 250, 400, 200, 480, 300, 480, color_gray);
    bj_draw_filled_triangle(bmp, 300, 420, 250, 480, 350, 480, color_dark_gray);
    bj_draw_filled_triangle(bmp, 350, 390, 300, 480, 400, 480, color_light_gray);
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(BJ_VIDEO_SYSTEM, &p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("Simple Text", 100, 100, 500, 500, 0);

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    draw(bj_get_framebuffer(renderer));

    bj_present(renderer, window);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_sleep(300);

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_end();
    return status;
}

