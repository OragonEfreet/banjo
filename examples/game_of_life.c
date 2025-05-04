////////////////////////////////////////////////////////////////////////////////
/// \example game_of_life.c
/// Complete example of the classical Game of Life.
///
/// Once the screen displays, click on any surface to draw "alive" cells and
/// release to let the simulation animate.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/window.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CANVAS_WIDTH 200
#define CANVAS_HEIGHT 200

#define BMP_MODE BJ_PIXEL_MODE_BGR24

#define STEP_MS 50

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
    if (painting) {
        bj_bitmap_put_pixel(presentation_fb,
            ((float)x / (float)SCREEN_WIDTH) * CANVAS_WIDTH,
            ((float)y / (float)SCREEN_HEIGHT) * CANVAS_HEIGHT,
            draw_color_1
        );
    }
}


void draw(size_t step) {
    bj_bitmap_clear(draw_fb);

    for (size_t x = 0; x < CANVAS_WIDTH; ++x) {
        for (size_t y = 0; y < CANVAS_HEIGHT; ++y) {
            size_t alive_neigh = 0;

            // Define the range of neighboring cells
            size_t xmin = x > 0 ? x - 1 : 0;
            size_t xmax = x < CANVAS_WIDTH - 1 ? x + 1 : CANVAS_WIDTH - 1;
            size_t ymin = y > 0 ? y - 1 : 0;
            size_t ymax = y < CANVAS_HEIGHT - 1 ? y + 1 : CANVAS_HEIGHT - 1;

            // Count alive neighbors
            for (size_t nx = xmin; nx <= xmax; ++nx) {
                for (size_t ny = ymin; ny <= ymax; ++ny) {
                    if (!(nx == x && ny == y)) // Exclude the cell itself
                        alive_neigh += bj_bitmap_get(presentation_fb, nx, ny) != back_color;
                }
            }

            // Update cell state
            if (bj_bitmap_get(presentation_fb, x, y) != back_color) {
                if (alive_neigh > 3 || alive_neigh < 2) {
                    bj_bitmap_put_pixel(draw_fb, x, y, back_color);
                }
                else {
                    bj_bitmap_put_pixel(draw_fb, x, y, draw_color_1);
                }
            }
            else {
                if (alive_neigh == 3) {
                    bj_bitmap_put_pixel(draw_fb, x, y, draw_color_2);
                }
            }
        }
    }


    // Swap presentation and draw bitmap
    bj_bitmap* temp = presentation_fb;
    presentation_fb = draw_fb;
    draw_fb = temp;
}

int main(int argc, char* argv[]) {
    presentation_fb = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, BMP_MODE, 0);
    draw_fb = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, BMP_MODE, 0);

    draw_color_1 = bj_pixel_value(BMP_MODE, 0x80, 0x80, 0x00);
    draw_color_2 = bj_pixel_value(BMP_MODE, 0x00, 0x80, 0x00);
    back_color = bj_pixel_value(BMP_MODE, 0x40, 0x40, 0x40);

    bj_bitmap_set_clear_color(presentation_fb, back_color);
    bj_bitmap_set_clear_color(draw_fb, back_color);

    bj_bitmap_clear(presentation_fb);

    bj_error* p_error = 0;

    if (!bj_system_init(&p_error)) {
        return 1;
    }

    bj_window* window = bj_window_new("Game of Life", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    bj_window_set_key_event(window, key_event);
    bj_window_set_button_event(window, button_event);
    bj_window_set_cursor_event(window, cursor_event);


    bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);

    size_t step = 0;
    while (!bj_window_should_close(window)) {
        bj_poll_events();

        bool need_update = false;
        if (!painting /* && ticks_since_last >= STEP_MS*/) {
            draw(step++);
            //ticks = current_ticks;
            need_update = true;
        }

        if (need_update || painting) {
            bj_bitmap_blit_stretched(presentation_fb, 0, framebuffer, 0);
            bj_window_update_framebuffer(window);

            //SDL_RenderClear(renderer);
        }

        bj_sleep(16); // TODO Use a better timer system
    }


    bj_window_del(window);
    bj_system_dispose(0);

    return 0;
}
