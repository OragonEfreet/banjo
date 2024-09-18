////////////////////////////////////////////////////////////////////////////////
/// \example game_of_life.c
/// Complete example of the classical Game of Life.
///
/// Once the screen displays, click on any surface to draw "alive" cells and
/// release to let the simulation animate.
////////////////////////////////////////////////////////////////////////////////
#include "banjo/color.h"
#include <banjo/bitmap.h>
#include <banjo/memory.h>

#include "sdl_helpers.h"
#include <SDL3/SDL_main.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CANVAS_WIDTH 200
#define CANVAS_HEIGHT 200

#define BMP_MODE BJ_PIXEL_MODE_BGR24

#define STEP_MS 50

// We will draw on `draw_fb` and display on `presentation_fb`
bj_bitmap* draw_fb         = 0;
bj_bitmap* presentation_fb = 0;
uint32_t draw_color_1      = 0;
uint32_t draw_color_2      = 0;
uint32_t back_color        = 0;

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
                } else {
                    bj_bitmap_put_pixel(draw_fb, x, y, draw_color_1);
                }
            } else {
                if (alive_neigh == 3) {
                    bj_bitmap_put_pixel(draw_fb, x, y, draw_color_2);
                }
            }
        }
    }


    // Swap presentation and draw bitmap
    bj_bitmap* temp = presentation_fb;
    presentation_fb = draw_fb;
    draw_fb         = temp;
}

int main(int argc, char* argv[]) {
    presentation_fb = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, BMP_MODE, 0);
    draw_fb         = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, BMP_MODE, 0);

    draw_color_1 = bj_pixel_value(BMP_MODE, 0x80, 0x80, 0x00);
    draw_color_2 = bj_pixel_value(BMP_MODE, 0x00, 0x80, 0x00);
    back_color   = bj_pixel_value(BMP_MODE, 0x40, 0x40, 0x40);

    bj_bitmap_set_clear_color(presentation_fb, back_color);
    bj_bitmap_set_clear_color(draw_fb, back_color);

    bj_bitmap_clear(presentation_fb);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window           = SDL_CreateWindow("Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer       = SDL_CreateRenderer(window, 0);
    SDL_PixelFormat pixel_format = bj_pixel_mode_to_sdl(BMP_MODE);
    SDL_Texture* texture         = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    bool quit = false;
    SDL_Event e;
    Uint64 ticks = SDL_GetTicks();
    size_t step = 0;
    bool painting = false;
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {

            switch(e.type) {
                // User requests quit
                case SDL_EVENT_KEY_UP:
                    if(painting) {
                        painting = false;
                    } else {
                        quit = true;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    painting = true;
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if(painting) {
                        bj_bitmap_put_pixel(presentation_fb,
                            ((float)e.button.x / (float)SCREEN_WIDTH) * CANVAS_WIDTH,
                            ((float)e.button.y / (float)SCREEN_HEIGHT) * CANVAS_HEIGHT,
                            BJ_COLOR_GREEN
                        );
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        painting = false;
                    }
                    break;
                }
        }
        Uint64 current_ticks    = SDL_GetTicks();
        Uint64 ticks_since_last = current_ticks - ticks;

        bool need_update = false;
        if(!painting && ticks_since_last >= STEP_MS) {
            draw(step++);
            ticks = current_ticks;
            need_update = true;
        }

        if(need_update || painting) {
            SDL_UpdateTexture(texture, 0, bj_bitmap_pixels(presentation_fb), bj_bitmap_stride(presentation_fb));
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, 0, 0);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
