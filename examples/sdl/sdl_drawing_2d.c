////////////////////////////////////////////////////////////////////////////////
/// \example drawing_2d.c
/// Drawing functions demo.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "sdl_helpers.h"
#include <SDL3/SDL_main.h>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

#define CANVAS_WIDTH 500
#define CANVAS_HEIGHT 500

void draw(bj_bitmap* bmp) {
    bj_bitmap_clear(bmp);

    const uint32_t color_aquamarine = bj_bitmap_pixel_value(bmp, 0xFF, 0x00, 0xFF);
    const uint32_t color_cyan = bj_bitmap_pixel_value(bmp, 0x7F, 0xFF, 0xD4);
    const uint32_t color_white = bj_bitmap_pixel_value(bmp, 0xFF, 0xFF, 0xFF);

    // Draw pixels individually
    for (size_t x = 10 ; x < 490 ; ++x) {
        if(x % 7 == 0) {
            bj_info("%ld / %ld", x, 10);
            bj_bitmap_put_pixel(bmp, x, 10, color_aquamarine);
        }
    }

    // Draw lines (shapes a banjo)
    bj_pixel points[18] = { 
        {100, 20},  {95,  25},  {95,  50},  {100, 55},  {100, 100}, {95,  100},
        {75,  120}, {75,  145}, {95,  165}, {120, 165}, {140, 145}, {140, 120},
        {120, 100}, {115, 100}, {115, 55},  {120, 50},  {120, 25},  {115, 20},
    };

    for(size_t p = 0 ; p < 18 ; ++p) {
        bj_bitmap_draw_line(bmp,
            points[p],
            points[(p+1)%18],
            color_cyan
        );
    }

    // Draw triangles (shapes a fox)
    bj_pixel verts[] =  {
        {330, 270}, {270, 210}, {210, 270}, {210, 150}, {390, 210}, {450, 270},
        {450, 150}, {180, 330}, {270, 390}, {390, 390}, {480, 330}, {330, 450},
        {300, 480}, {360, 480},
    };

    size_t tris[13][3] = {
        {0, 1, 2},   {0, 2, 3},   {0, 4, 5},  {0, 1, 4},  {4, 6, 5}, {2, 8, 7},
        {0, 8, 2},   {0, 5, 9},   {9, 5, 10}, {8, 9, 11}, {8, 11, 12},
        {9, 13, 11}, {11, 12, 13},
    };


    for(size_t t = 0 ; t < 13 ; ++t) {
        bj_bitmap_draw_triangle(bmp,
            verts[tris[t][0]], verts[tris[t][1]], verts[tris[t][2]],
            color_white
        );
    }
}

int main(int argc, char* argv[]) {
    bj_bitmap* bmp = bj_bitmap_new(CANVAS_WIDTH, CANVAS_HEIGHT, BJ_PIXEL_MODE_BGR24, 0);
    /* bj_bitmap_set_clear_color(bmp, BJ_COLOR_DARK_GRAY); */ // TODO

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("Banjo - 2D drawing", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    SDL_PixelFormat pixel_format = bj_pixel_mode_to_sdl(bj_bitmap_mode(bmp));
    SDL_Texture* texture   = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    draw(bmp);

    SDL_UpdateTexture(texture, 0, bj_bitmap_pixels(bmp), bj_bitmap_stride(bmp));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_EVENT_KEY_UP) {
                quit = true;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
