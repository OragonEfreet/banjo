////////////////////////////////////////////////////////////////////////////////
/// \example game_of_life.c
/// Complete example of the classical Game of Life.
///
/// Once the screen displays, click on any surface to draw "alive" cells and
/// release to let the simulation animate.
////////////////////////////////////////////////////////////////////////////////
#include "banjo/color.h"
#include <banjo/oldbmp.h>
#include <banjo/memory.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CANVAS_WIDTH 200
#define CANVAS_HEIGHT 200

#define DRAW_COLOR_1 BJ_COLOR_OLIVE
#define DRAW_COLOR_2 BJ_COLOR_DARK_GREEN
#define BACK_COLOR BJ_COLOR_DARK_GRAY

#define STEP_MS 50

// We will draw on `draw_fb` and display on `presentation_fb`
bj_oldbmp* draw_fb         = 0;
bj_oldbmp* presentation_fb = 0;

void draw(usize step) {
    bj_oldbmp_clear(draw_fb);

    for (usize x = 0; x < CANVAS_WIDTH; ++x) {
        for (usize y = 0; y < CANVAS_HEIGHT; ++y) {
            usize alive_neigh = 0;

            // Define the range of neighboring cells
            usize xmin = x > 0 ? x - 1 : 0;
            usize xmax = x < CANVAS_WIDTH - 1 ? x + 1 : CANVAS_WIDTH - 1;
            usize ymin = y > 0 ? y - 1 : 0;
            usize ymax = y < CANVAS_HEIGHT - 1 ? y + 1 : CANVAS_HEIGHT - 1;

            // Count alive neighbors
            for (usize nx = xmin; nx <= xmax; ++nx) {
                for (usize ny = ymin; ny <= ymax; ++ny) {
                    if (!(nx == x && ny == y)) // Exclude the cell itself
                        alive_neigh += bj_oldbmp_get(presentation_fb, nx, ny) != BACK_COLOR;
                }
            }

            // Update cell state
            if (bj_oldbmp_get(presentation_fb, x, y) != BACK_COLOR) {
                if (alive_neigh > 3 || alive_neigh < 2) {
                    bj_oldbmp_put(draw_fb, x, y, BACK_COLOR);
                } else {
                    bj_oldbmp_put(draw_fb, x, y, DRAW_COLOR_1);
                }
            } else {
                if (alive_neigh == 3) {
                    bj_oldbmp_put(draw_fb, x, y, DRAW_COLOR_2);
                }
            }
        }
    }


    // Swap presentation and draw oldbmp
    bj_oldbmp* temp = presentation_fb;
    presentation_fb      = draw_fb;
    draw_fb              = temp;
}

int main(int argc, char* argv[]) {
    presentation_fb = bj_oldbmp_new(CANVAS_WIDTH, CANVAS_HEIGHT);
    draw_fb         = bj_oldbmp_new(CANVAS_WIDTH, CANVAS_HEIGHT);

    bj_oldbmp_set_clear_color(presentation_fb, BACK_COLOR);
    bj_oldbmp_set_clear_color(draw_fb, BACK_COLOR);

    bj_oldbmp_clear(presentation_fb);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    bool quit = false;
    SDL_Event e;
    Uint64 ticks = SDL_GetTicks();
    usize step = 0;
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
                        bj_oldbmp_put(presentation_fb,
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
            SDL_UpdateTexture(texture, 0, bj_oldbmp_data(presentation_fb), CANVAS_WIDTH * sizeof (u32));
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
