////////////////////////////////////////////////////////////////////////////////
/// \example load_bmp_file.c
/// Loading BMP files into \ref bj_bitmap.
///
/// \details Use \ref bj_bitmap_new_from_file to load bitmap data into a \ref bj_bitmap.
////////////////////////////////////////////////////////////////////////////////
#include "sdl_helpers.h"

#include <banjo/bitmap.h>
#include <banjo/color.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

int main(int argc, char* argv[]) {

    bj_error* error = 0;
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/lena.bmp", &error); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/all_gray.bmp", &error); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/dots.bmp", &error); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/lena.bmp", &error); */

    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/blackbuck.bmp", &error); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/bmp_24.bmp", &error); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", &error); */
    bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/greenland_grid_velo.bmp", &error);
    /* bj_bitmap* p_bitmap = bj_bitmap_new_from_file(BANJO_ASSETS_DIR"/bmp/snail.bmp", &error); */


    usize window_w = bj_bitmap_width(p_bitmap);
    usize window_h = bj_bitmap_height(p_bitmap);

    if(error) {
        bj_err("Error %x: %s", error->code, error->message);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_PixelFormat pixel_format = bj_pixel_format_to_sdl(bj_bitmap_encoding(p_bitmap));

    SDL_Window* window     = SDL_CreateWindow("Load BMP Files - Banjo", window_w, window_h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, window_w, window_h);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_UpdateTexture(texture, 0, bj_bitmap_pixels(p_bitmap), bj_bitmap_stride(p_bitmap));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);

    bj_bitmap_del(p_bitmap);

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


