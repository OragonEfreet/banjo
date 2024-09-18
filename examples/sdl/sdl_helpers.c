#include "sdl_helpers.h"

#include <banjo/log.h>
#include <banjo/pixel.h>

#include <SDL3/SDL_pixels.h>

void sdl_what_is_the_format(const char* p_path) {
    SDL_Surface* surface = SDL_LoadBMP(p_path);
    if(surface) {
        bj_info("Expected surface: %s", SDL_GetPixelFormatName(surface->format));
    }

    SDL_DestroySurface(surface);
}

int bj_pixel_mode_to_sdl(int mode) {
    switch(mode) {
        case BJ_PIXEL_MODE_INDEXED_1:
            return SDL_PIXELFORMAT_INDEX1LSB; // OR MSB, don't care
        case BJ_PIXEL_MODE_INDEXED_4:
            return SDL_PIXELFORMAT_INDEX4LSB; // OR MSB, don't care
        case BJ_PIXEL_MODE_RGB565:
            return SDL_PIXELFORMAT_RGB565; // OK
        case BJ_PIXEL_MODE_XRGB1555:
            return SDL_PIXELFORMAT_XRGB1555; // OK
        case BJ_PIXEL_MODE_XRGB8888:
            return SDL_PIXELFORMAT_XRGB8888;
        case BJ_PIXEL_MODE_BGR24:
            return SDL_PIXELFORMAT_BGR24;
        case BJ_PIXEL_MODE_INDEXED_8:
            return SDL_PIXELFORMAT_INDEX8;
        default: break;
    }
    return SDL_PIXELFORMAT_UNKNOWN;
}

int sdl_display_bitmap(bj_bitmap* p_bitmap, const char* title, bool pause_on_display) {

    SDL_PixelFormat pixel_format = bj_pixel_mode_to_sdl(bj_bitmap_mode(p_bitmap));

    if(pixel_format == SDL_PIXELFORMAT_UNKNOWN) {
        bj_warn("%s: unknown pixel format", title);
    }

    const size_t bitmap_w = bj_bitmap_width(p_bitmap);
    const size_t bitmap_h = bj_bitmap_height(p_bitmap);

    size_t window_w = bitmap_w;
    size_t window_h = bitmap_h;

    while((window_w + window_h)*(window_w + window_h) < 800*800) {
        window_w *=2;
        window_h *=2;
        /* bj_debug("Upscale %ld/%ld BMP %ld/%ld", bitmap_w, bitmap_h, window_w, window_h); */
    }

    SDL_Window* window     = SDL_CreateWindow(title, window_w, window_h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);

    SDL_Texture* texture  = SDL_CreateTexture(
        renderer,
        pixel_format,
        SDL_TEXTUREACCESS_STREAMING,
        bitmap_w, bitmap_h
    );
    if(texture == 0) {
        bj_err("[SDL] %s: %s", title, SDL_GetError());
        /* SDL_DestroyRenderer(renderer); */
        /* SDL_DestroyWindow(window); */
        /* return 0; */
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_UpdateTexture(texture, 0, 
        bj_bitmap_pixels(p_bitmap),
        bj_bitmap_stride(p_bitmap)
    );
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);

    if(pause_on_display) {
        bool quit = false;
        SDL_Event e;
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_EVENT_KEY_UP) {
                    quit = true;
                }
            }
        }
    } else {
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 1;
}


