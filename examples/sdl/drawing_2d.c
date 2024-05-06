#include "banjo/color.h"
#include <banjo/framebuffer.h>
#include <banjo/memory.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CANVAS_WIDTH 500
#define CANVAS_HEIGHT 500

void draw(bj_framebuffer* fb) {
    bj_framebuffer_clear(fb);

    // Draw pixels individually
    for (usize x = 10 ; x < 490 ; ++x) {
        if(x % 7 == 0) {
            bj_framebuffer_put(fb, x, 10, BJ_COLOR_AQUAMARINE);
        }
    }

    // Draw lines (sapes a banjo)
    bj_pixel points[18] = { 
        {100, 20},  {95,  25},  {95,  50},  {100, 55},  {100, 100}, {95,  100},
        {75,  120}, {75,  145}, {95,  165}, {120, 165}, {140, 145}, {140, 120},
        {120, 100}, {115, 100}, {115, 55},  {120, 50},  {120, 25},  {115, 20},
    };

    for(usize p = 0 ; p < 18 ; ++p) {
        bj_framebuffer_draw_line(fb,
            points[p],
            points[(p+1)%18],
            BJ_COLOR_CYAN
        );
    }

    // Draw triangles (shapes a fox)
    bj_pixel verts[] =  {
        {330, 270}, {270, 210}, {210, 270}, {210, 150}, {390, 210}, {450, 270},
        {450, 150}, {180, 330}, {270, 390}, {390, 390}, {480, 330}, {330, 450},
        {300, 480}, {360, 480},
    };

    usize tris[13][3] = {
        {0, 1, 2},   {0, 2, 3},   {0, 4, 5},  {0, 1, 4},  {4, 6, 5}, {2, 8, 7},
        {0, 8, 2},   {0, 5, 9},   {9, 5, 10}, {8, 9, 11}, {8, 11, 12},
        {9, 13, 11}, {11, 12, 13},
    };


    for(usize t = 0 ; t < 13 ; ++t) {
        bj_framebuffer_draw_triangle(fb,
            verts[tris[t][0]], verts[tris[t][1]], verts[tris[t][2]],
            BJ_COLOR_WHITE
        );
    }
}

int main(int argc, char* argv[]) {
    bj_framebuffer* fb = bj_new(framebuffer, default, CANVAS_WIDTH, CANVAS_HEIGHT);
    bj_framebuffer_set_clear_color(fb, BJ_COLOR_DARK_GRAY);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("Banjo - 2D drawing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);

    draw(fb);

    SDL_UpdateTexture(texture, 0, bj_framebuffer_data(fb), CANVAS_WIDTH * sizeof (u32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_KEYUP) {
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
