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

#include "sdl_helpers.h"
#include <SDL3/SDL_main.h>

#include <stdio.h>

#define WINDOW_W 10
#define WINDOW_H 10

int main(int argc, char* argv[]) {

    bj_error* error = 0;


    bj_bitmap* p_bitmap = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_BGR24, 0);
    /* bj_bitmap* p_bitmap = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_RGB565, 0); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_XRGB8888, 0); */
    /* bj_bitmap* p_bitmap = bj_bitmap_new(WINDOW_W, WINDOW_H, BJ_PIXEL_MODE_XRGB1555, 0); */
    const uint32_t color = bj_bitmap_pixel_value(p_bitmap, 0x11, 0x22, 0x33);
    printf("color: %x\n", color);
    const uint32_t red = bj_bitmap_pixel_value(p_bitmap, 255, 0, 0);
    printf("red: %x\n", red);
    const uint32_t green = bj_bitmap_pixel_value(p_bitmap, 0, 255, 0);
    printf("green: %x\n", green);
    const uint32_t blue = bj_bitmap_pixel_value(p_bitmap, 0, 0, 255);
    printf("blue: %x\n", blue);

    bj_bitmap_put_pixel(p_bitmap, 0, 0, red);
    bj_bitmap_put_pixel(p_bitmap, 1, 1, green);
    bj_bitmap_put_pixel(p_bitmap, 2, 2, blue);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window     = SDL_CreateWindow("Bitmaps - Banjo", WINDOW_W * 10, WINDOW_H * 10, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
    SDL_PixelFormat pixel_format = bj_pixel_mode_to_sdl(bj_bitmap_mode(p_bitmap));
    SDL_Texture* texture   = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);
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


