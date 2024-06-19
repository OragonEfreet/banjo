////////////////////////////////////////////////////////////////////////////////
/// \example sprite_animation.c
/// A quick example of animating a sprite.
///
/// This sprite animation is performed using \ref bj_bitmap_new_from_file to
/// load the sprite sheet in memory and \ref bj_bitmap_blit to display each
/// frame over time.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/color.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <SDL2/SDL.h>

#define SPRITE_W 24
#define SPRITE_H 24
#define FRAMES 7

#define WINDOW_W 240
#define WINDOW_H 240


int main(int argc, char* argv[]) {

    bj_bitmap* bmp_rendering = bj_bitmap_new(SPRITE_W, SPRITE_H);
    bj_bitmap_set_clear_color(bmp_rendering, BJ_COLOR_RED);
    bj_bitmap_clear(bmp_rendering);

    bj_bitmap* bmp_sprite_sheet = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("sprite sheet - Banjo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SPRITE_W, SPRITE_H);


    usize frame_count = 1;

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_KEYUP) {
                quit = true;
            }
        }

        bj_bitmap_blit(bmp_sprite_sheet, &(bj_rect){
            .x = frame_count * SPRITE_W,
            .w = 24, .h = 24
        }, bmp_rendering, &(bj_rect){.x = 0, .y = 0});

        SDL_UpdateTexture(texture, 0, bj_bitmap_data(bmp_rendering), SPRITE_W * sizeof (u32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, 0, 0);
        SDL_RenderPresent(renderer);

        SDL_Delay(120);
        if(++frame_count >= FRAMES) {
            frame_count = 1;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    bj_bitmap_del(bmp_sprite_sheet);
    bj_bitmap_del(bmp_rendering);



    return 0;
}


