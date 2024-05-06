#include <banjo/framebuffer.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    const char* folder = BANJO_ASSETS_DIR;
    usize folder_len = strlen(folder);
    
    const char* bmp_files[7] = {
        "bmp/all_gray.bmp",
        "bmp/blackbuck.bmp",
        "bmp/bmp_24.bmp",
        "bmp/dots.bmp",
        "bmp/greenland_grid_velo.bmp",
        "bmp/lena.bmp",
        "bmp/snail.bmp",
    };

    for (usize f = 0 ; f < 7 ; ++f) {
        const char* bmp_file = bmp_files[f];

        char* bmp_path = bj_malloc(sizeof(char) * (folder_len + strlen(bmp_file) + 2));
        sprintf(bmp_path, "%s/%s", folder, bmp_file);

        bj_info("Opening %s", bmp_path); 
        bj_framebuffer* fb = bj_new(framebuffer, from_file, bmp_path, 0);

        bj_info("Size: %d x %d", fb->width, fb->height);

        bj_free(bmp_path);
    }

    return 0;
}
