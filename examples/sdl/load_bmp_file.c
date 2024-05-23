#include <banjo/bitmap.h>
#include <banjo/color.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    const char* folder = BANJO_ASSETS_DIR;
    usize folder_len = strlen(folder);

    const char* bmp_files[] = {
        /* "bmp/all_gray.bmp", */
        "bmp/blackbuck.bmp",
        "bmp/bmp_24.bmp",
        "bmp/dots.bmp",
        "bmp/greenland_grid_velo.bmp",
        "bmp/lena.bmp",
        "bmp/snail.bmp",
    };


    usize len = sizeof(bmp_files) / sizeof(bmp_files[0]);

    for(usize f = 0 ; f < len ; ++f) {
        const char* bmp_file = bmp_files[f];
        char* bmp_path = bj_malloc(sizeof(char) * (folder_len + strlen(bmp_file) + 2));
        sprintf(bmp_path, "%s/%s", folder, bmp_file);
        bj_info("Opening %s", bmp_path); 

        bj_error* p_error = 0;

        bj_bitmap* bmp = 0;
        bmp = bj_new(bitmap, from_file, bmp_path, &p_error);
        bj_free(bmp_path);
        if(p_error) {
            bj_error("While loading '%s': %s (0x%08X)",
                bmp_file,
                p_error->message,
                p_error->code
            );
        } else {
            bj_info("Size: %d x %d", bmp->width, bmp->height);

            SDL_Window* window     = SDL_CreateWindow(bmp_file, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, bmp->width, bmp->height, SDL_WINDOW_SHOWN);
            SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
            SDL_Texture* texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, bmp->width, bmp->height);

            SDL_UpdateTexture(texture, 0, bj_bitmap_data(bmp), bmp->width * sizeof (u32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, 0, 0);
            SDL_RenderPresent(renderer);

            bool quit = false;
            if(quit) {
                SDL_Delay(150);
            } else {
                SDL_Event e;
                while (!quit) {
                    while (SDL_PollEvent(&e) != 0) {
                        if(e.type == SDL_KEYUP) {
                            quit = true;
                        }
                    }
                }
            }

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
        }
        bj_clear_error(&p_error);
        bj_del(bitmap, bmp);
    }

    SDL_Quit();

    return 0;
}


