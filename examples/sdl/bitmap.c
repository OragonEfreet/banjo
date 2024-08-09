////////////////////////////////////////////////////////////////////////////////
/// \example bitmap.c
/// Create a Bitmap and display it onscreen without renderer.
///
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/pixel.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_W 800
#define WINDOW_H 600

int main(int argc, char* argv[]) {

    bj_error* error = 0;
    bj_bitmap* p_bitmap = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_FORMAT_BGR24);

    bj_bitmap_put_rgb(p_bitmap, 10, 10, 255, 0, 0);
    bj_bitmap_put_rgb(p_bitmap, 15, 10, 0, 255, 0);
    bj_bitmap_put_rgb(p_bitmap, 20, 10, 0, 0, 255);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("Bitmaps - Banjo", WINDOW_W, WINDOW_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_UpdateTexture(texture, 0, bj_bitmap_pixels(p_bitmap), WINDOW_W * 3);
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


