#include <banjo/bitmap.h>
#include <banjo/color.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <SDL2/SDL.h>

#define WINDOW_W 800
#define WINDOW_H 600

int main(int argc, char* argv[]) {

    bj_bitmap* bmp_rendering = bj_new(bitmap, default, WINDOW_W, WINDOW_H);
    bj_bitmap_set_clear_color(bmp_rendering, BJ_COLOR_RED);
    bj_bitmap_clear(bmp_rendering);

    bj_bitmap* bmp_blackbuck_512_512 = bj_new(bitmap, from_file, BANJO_ASSETS_DIR"/bmp/blackbuck.bmp", 0);
    bj_bitmap* bmp_greenland_grid_velo_762_1309 = bj_new(bitmap, from_file, BANJO_ASSETS_DIR"/bmp/greenland_grid_velo.bmp", 0);
    bj_bitmap* bmp_lena_512_512 = bj_new(bitmap, from_file, BANJO_ASSETS_DIR"/bmp/lena.bmp", 0);
    bj_bitmap* bmp_snail_256_256 = bj_new(bitmap, from_file, BANJO_ASSETS_DIR"/bmp/snail.bmp", 0);

    bj_bitmap_blit(bmp_greenland_grid_velo_762_1309, &(bj_rect){.x = 0, .y = 0, .w = 762, .h = 1309}, bmp_rendering, &(bj_rect){.x = 20, .y = 0});
    bj_bitmap_blit(bmp_blackbuck_512_512, &(bj_rect){.x = 100, .y = 100, .w = 512, .h = 512}, bmp_rendering, &(bj_rect){.x = 100, .y = 200});
    bj_bitmap_blit(bmp_snail_256_256, &(bj_rect){.x = 0, .y = 0, .w = 256, .h = 256}, bmp_rendering, &(bj_rect){.x = 500, .y = 130});

    bj_del(bitmap, bmp_snail_256_256);
    bj_del(bitmap, bmp_lena_512_512);
    bj_del(bitmap, bmp_greenland_grid_velo_762_1309);
    bj_del(bitmap, bmp_blackbuck_512_512);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("bitmap_blitting - Banjo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);

    SDL_UpdateTexture(texture, 0, bj_bitmap_data(bmp_rendering), WINDOW_W * sizeof (u32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);

    bj_del(bitmap, bmp_rendering);

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


