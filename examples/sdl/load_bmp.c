////////////////////////////////////////////////////////////////////////////////
/// \example load_bmp.c
/// Loading BMP files into \ref bj_bitmap.
///
/// \details Use \ref bj_bitmap_new_from_file to load bitmap data into a \ref bj_bitmap.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "sdl_helpers.h"
#include <SDL3/SDL_main.h>

#include <string.h>

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    size_t total_tries       = 0;
    size_t total_ok          = 0;
    const char* bmp_files[] = {
        "/bmp/all_gray.bmp",
        "/bmp/blackbuck.bmp",
        "/bmp/bmp_24.bmp",
        "/bmp/dots.bmp",
        "/bmp/gabe-idle-run.bmp",
        "/bmp/greenland_grid_velo.bmp",
        "/bmp/lena.bmp",
        "/bmp/snail.bmp",
        "/bmp/test/valid/1bpp-1x1.bmp",
        "/bmp/test/valid/1bpp-320x240.bmp",
        "/bmp/test/valid/1bpp-320x240-color.bmp",
        "/bmp/test/valid/1bpp-320x240-overlappingcolor.bmp",
        "/bmp/test/valid/1bpp-321x240.bmp",
        "/bmp/test/valid/1bpp-322x240.bmp",
        "/bmp/test/valid/1bpp-323x240.bmp",
        "/bmp/test/valid/1bpp-324x240.bmp",
        "/bmp/test/valid/1bpp-325x240.bmp",
        "/bmp/test/valid/1bpp-326x240.bmp",
        "/bmp/test/valid/1bpp-327x240.bmp",
        "/bmp/test/valid/1bpp-328x240.bmp",
        "/bmp/test/valid/1bpp-329x240.bmp",
        "/bmp/test/valid/1bpp-330x240.bmp",
        "/bmp/test/valid/1bpp-331x240.bmp",
        "/bmp/test/valid/1bpp-332x240.bmp",
        "/bmp/test/valid/1bpp-333x240.bmp",
        "/bmp/test/valid/1bpp-334x240.bmp",
        "/bmp/test/valid/1bpp-335x240.bmp",
        "/bmp/test/valid/1bpp-topdown-320x240.bmp",
        "/bmp/test/valid/24bpp-1x1.bmp",
        "/bmp/test/valid/24bpp-320x240.bmp",
        "/bmp/test/valid/24bpp-321x240.bmp",
        "/bmp/test/valid/24bpp-322x240.bmp",
        "/bmp/test/valid/24bpp-323x240.bmp",
        "/bmp/test/valid/24bpp-imagesize-zero.bmp",
        "/bmp/test/valid/24bpp-topdown-320x240.bmp",
        "/bmp/test/valid/32bpp-101110-320x240.bmp",
        "/bmp/test/valid/32bpp-1x1.bmp",
        "/bmp/test/valid/32bpp-320x240.bmp",
        "/bmp/test/valid/32bpp-888-optimalpalette-320x240.bmp",
        "/bmp/test/valid/32bpp-optimalpalette-320x240.bmp",
        "/bmp/test/valid/32bpp-topdown-320x240.bmp",
        "/bmp/test/valid/4bpp-1x1.bmp",
        "/bmp/test/valid/4bpp-320x240.bmp",
        "/bmp/test/valid/4bpp-321x240.bmp",
        "/bmp/test/valid/4bpp-322x240.bmp",
        "/bmp/test/valid/4bpp-323x240.bmp",
        "/bmp/test/valid/4bpp-324x240.bmp",
        "/bmp/test/valid/4bpp-325x240.bmp",
        "/bmp/test/valid/4bpp-326x240.bmp",
        "/bmp/test/valid/4bpp-327x240.bmp",
        "/bmp/test/valid/4bpp-topdown-320x240.bmp",
        "/bmp/test/valid/555-1x1.bmp",
        "/bmp/test/valid/555-320x240.bmp",
        "/bmp/test/valid/555-321x240.bmp",
        "/bmp/test/valid/565-1x1.bmp",
        "/bmp/test/valid/565-320x240.bmp",
        "/bmp/test/valid/565-320x240-topdown.bmp",
        "/bmp/test/valid/565-321x240.bmp",
        "/bmp/test/valid/565-321x240-topdown.bmp",
        "/bmp/test/valid/565-322x240.bmp",
        "/bmp/test/valid/565-322x240-topdown.bmp",
        "/bmp/test/valid/8bpp-1x1.bmp",
        "/bmp/test/valid/8bpp-320x240.bmp",
        "/bmp/test/valid/8bpp-321x240.bmp",
        "/bmp/test/valid/8bpp-322x240.bmp",
        "/bmp/test/valid/8bpp-323x240.bmp",
        "/bmp/test/valid/8bpp-colorsimportant-two.bmp",
        "/bmp/test/valid/8bpp-colorsused-zero.bmp",
        "/bmp/test/valid/8bpp-topdown-320x240.bmp",
        "/bmp/test/valid/rle4-absolute-320x240.bmp",
        "/bmp/test/valid/rle4-alternate-320x240.bmp",
        "/bmp/test/valid/rle4-delta-320x240.bmp",
        "/bmp/test/valid/rle4-encoded-320x240.bmp",
        "/bmp/test/valid/rle8-absolute-320x240.bmp",
        "/bmp/test/valid/rle8-blank-160x120.bmp",
        "/bmp/test/valid/rle8-delta-320x240.bmp",
        "/bmp/test/valid/rle8-encoded-320x240.bmp",
        "/bmp/test/valid/ spaces in  filename.bmp",
    };
    size_t n_files = sizeof(bmp_files) / sizeof(bmp_files[0]);

    const size_t asset_dir_strlen = strlen(BANJO_ASSETS_DIR);

    bool pause_on_display = true;
    for (size_t f = 0 ; f < n_files ; ++f) {
        total_tries +=1 ;
        const char* bmp_file = bmp_files[f];
        const size_t bmp_file_strlen = strlen(bmp_file);
        const size_t bufsize = asset_dir_strlen + bmp_file_strlen + 1;

        char* bmp_path = bj_malloc(bufsize);
        bj_memset(bmp_path, 0, bufsize);
        bj_memcpy(bmp_path, BANJO_ASSETS_DIR, asset_dir_strlen);
        bj_memcpy(bmp_path+asset_dir_strlen, bmp_file, bmp_file_strlen);

        /* sdl_what_is_the_format(bmp_path); */

        bj_error* error = 0;
        bj_bitmap* p_bitmap = bj_bitmap_new_from_file(bmp_path, &error);
        bj_free(bmp_path);

        if(error) {
            bj_err("%s: Error %x: %s", bmp_file, error->code, error->message);
            bj_clear_error(&error);
            continue;
        }

        if(p_bitmap == 0) {
            bj_debug("%s: no bitmap loaded", bmp_file);
            continue;
        }

        if(sdl_display_bitmap(p_bitmap, bmp_file, pause_on_display)) {
            bj_info("%s OK", bmp_file);
            total_ok +=1;
        }
        pause_on_display = false;
        bj_bitmap_del(p_bitmap);
    }

    bj_info("%d/%d files read (%2.2f%)", total_ok, total_tries, ((float)total_ok / (float)total_tries) * 100.f);

    SDL_Quit();
    return 0;
}


