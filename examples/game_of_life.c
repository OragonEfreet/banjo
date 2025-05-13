////////////////////////////////////////////////////////////////////////////////
/// \example game_of_life.c
/// Complete example of the classical Game of Life.
///
/// Once the screen displays, click on any surface to draw "alive" cells and
/// release to let the simulation animate.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CANVAS_WIDTH 160
#define CANVAS_HEIGHT 120

// We will draw on `draw_fb` and display on `presentation_fb`
bj_bitmap* draw_fb = 0;
bj_bitmap* presentation_fb = 0;
uint32_t draw_color_1 = 0;
uint32_t draw_color_2 = 0;
uint32_t back_color = 0;
bool painting = false;

void key_event(bj_window* p_window, bj_event_action action, bj_key key, int scancode) {
    (void)p_window;
    (void)action;
    (void)key;
    (void)scancode;

    if (painting) {
        painting = false;
    }
    else {
        bj_window_set_should_close(p_window);
    }
}

void button_event(bj_window* p_window, int button, bj_event_action action, int x, int y) {
    (void)p_window;
    (void)x;
    (void)y;
    if (action == BJ_PRESS) {
        painting = true;
    }
    else {
        if (button == BJ_BUTTON_LEFT) {
            painting = false;
        }
    }
}

void cursor_event(bj_window* p_window, int x, int y) {
    (void)p_window;
    if (painting) {
        bj_bitmap_put_pixel(presentation_fb,
            ((float)x / (float)SCREEN_WIDTH) * CANVAS_WIDTH,
            ((float)y / (float)SCREEN_HEIGHT) * CANVAS_HEIGHT,
            draw_color_1
        );
    }
}


void draw(size_t step) {
    (void)step;
    bj_bitmap_clear(draw_fb);

    for (size_t x = 0; x < CANVAS_WIDTH; ++x) {
        for (size_t y = 0; y < CANVAS_HEIGHT; ++y) {
            size_t alive_neigh = 0;

            // Define neighbor bounds (no wraparound)
            size_t xmin = x > 0 ? x - 1 : 0;
            size_t xmax = x < CANVAS_WIDTH - 1 ? x + 1 : CANVAS_WIDTH - 1;
            size_t ymin = y > 0 ? y - 1 : 0;
            size_t ymax = y < CANVAS_HEIGHT - 1 ? y + 1 : CANVAS_HEIGHT - 1;

            for (size_t nx = xmin; nx <= xmax; ++nx) {
                for (size_t ny = ymin; ny <= ymax; ++ny) {
                    if (!(nx == x && ny == y)) {
                        if (bj_bitmap_get(presentation_fb, nx, ny) != back_color) {
                            alive_neigh++;
                        }
                    }
                }
            }

            bool is_alive = bj_bitmap_get(presentation_fb, x, y) != back_color;

            if (is_alive) {
                if (alive_neigh == 2 || alive_neigh == 3) {
                    bj_bitmap_put_pixel(draw_fb, x, y, draw_color_1);
                } else {
                    bj_bitmap_put_pixel(draw_fb, x, y, back_color);
                }
            } else {
                if (alive_neigh == 3) {
                    bj_bitmap_put_pixel(draw_fb, x, y, draw_color_1);
                }
            }
        }
    }

    // Swap buffers
    bj_bitmap* temp = presentation_fb;
    presentation_fb = draw_fb;
    draw_fb = temp;
}

int main() {
    

    bj_error* p_error = 0;

    if (!bj_system_init(&p_error)) {
        return 1;
    }

    bj_window* window = bj_window_new("Game of Life", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    bj_window_set_key_event(window, key_event);
    bj_window_set_button_event(window, button_event);
    bj_window_set_cursor_event(window, cursor_event);
    bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);

    const bj_pixel_mode bmp_mode = bj_bitmap_mode(framebuffer);// TODO Doesn't work with framebuffer's mode.
    //const bj_pixel_mode bmp_mode = BJ_PIXEL_MODE_BGR24;

    presentation_fb = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, bmp_mode, 0);
    draw_fb = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, bmp_mode, 0);

    draw_color_1 = bj_pixel_value(bmp_mode, 0x80, 0x80, 0x00);
    draw_color_2 = bj_pixel_value(bmp_mode, 0x00, 0x80, 0x00);
    back_color = bj_pixel_value(bmp_mode, 0x40, 0x40, 0x40);

    bj_bitmap_set_clear_color(presentation_fb, back_color);
    bj_bitmap_set_clear_color(draw_fb, back_color);

    bj_bitmap_clear(presentation_fb);

    size_t step = 0;
    double time = bj_get_time();
    while (!bj_window_should_close(window)) {
        bj_poll_events();

        double current_time = bj_get_time();
        double time_since_last = current_time - time;

        bool need_update = false;
        if (!painting && time_since_last >= 0.05) {
            draw(step++);
            need_update = true;
        }

        if (need_update || painting) {
            bj_bitmap_blit_stretched(presentation_fb, 0, framebuffer, 0);
            bj_window_update_framebuffer(window);
            time = current_time;
        }

        bj_sleep(10);
    }


    bj_window_del(window);
    bj_system_dispose(0);

    return 0;
}
