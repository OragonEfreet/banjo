#include <banjo/bitmap.h>
#include <banjo/color.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <SDL2/SDL.h>

#define WINDOW_W 168
#define WINDOW_H 24

int main(int argc, char* argv[]) {

    bj_error* error = 0;
    bj_bitmap* bmp_lena_512_512 = bj_new(bitmap, from_file, BANJO_ASSETS_DIR"/bmp/gabe-idle-run.bmp", &error);

    if(error) {
        bj_error("Error %x: %s", error->code, error->message);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("Load BMP Files - Banjo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);

    SDL_UpdateTexture(texture, 0, 
        bj_bitmap_data(bmp_lena_512_512),
    WINDOW_W * sizeof (u32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);

    bj_del(bitmap, bmp_lena_512_512);

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


