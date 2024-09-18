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

#include "sdl_helpers.h"
#include <SDL3/SDL_main.h>

#define SPRITE_W 24
#define SPRITE_H 24
#define FRAMES 7

#define WINDOW_W 240
#define WINDOW_H 240


int main(int argc, char* argv[]) {


    bj_bitmap* bmp_rendering = bj_bitmap_new(SPRITE_W, SPRITE_H, BJ_PIXEL_MODE_BGR24, 0);
    bj_bitmap_set_clear_color(bmp_rendering, bj_bitmap_pixel_value(bmp_rendering, 0xFF, 0x00, 0x00));
    bj_bitmap_clear(bmp_rendering);

    bj_bitmap* bmp_sprite_sheet = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", 0);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("sprite sheet - Banjo", WINDOW_W, WINDOW_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    SDL_PixelFormat pixel_format = bj_pixel_mode_to_sdl(bj_bitmap_mode(bmp_rendering));
    SDL_Texture* texture         = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, SPRITE_W, SPRITE_H);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);


    size_t frame_count = 1;

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_EVENT_KEY_UP) {
                quit = true;
            }
        }

        bj_bitmap_blit(bmp_sprite_sheet, &(bj_rect){
            .x = frame_count * SPRITE_W,
            .w = 24, .h = 24
        }, bmp_rendering, &(bj_rect){.x = 0, .y = 0});

        SDL_UpdateTexture(texture, 0, bj_bitmap_pixels(bmp_rendering), bj_bitmap_stride(bmp_rendering));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, 0, 0);
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


