////////////////////////////////////////////////////////////////////////////////
/// \example load_bmp.c
/// Loading BMP files into \ref bj_bitmap.
///
/// \details Use \ref bj_bitmap_new_from_file to load bitmap data into a \ref bj_bitmap.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/rect.h>

#include <stdio.h>

int main(int argc, char* argv[]) {

    bj_bitmap* bmp = bj_bitmap_new(200, 255, 0, BJ_PIXEL_MODE_BGR24);

    printf("Loaded\n");

    bj_bitmap_del(bmp);

    printf("Unloaded\n");

    
    return 0;
}


