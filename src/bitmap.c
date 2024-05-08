#include <banjo/error.h>
#include <banjo/bitmap.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "dib.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef CENTERED_AXIS
#define S_X(x) ((p_bitmap->width / 2) + x)
#define S_Y(y) ((p_bitmap->height / 2) - y)
#else
#define S_X(x) (x)
#define S_Y(y) (y)
#endif
#define XY(x, y) (S_Y(y) * p_bitmap->width + S_X(x))
#define at(bmp, x, y) bmp[XY(x, y)]
#define put_pixel(bmp, x, y, c) at(bmp, x, y) = c

BANJO_EXPORT bj_bitmap* bj_bitmap_init_default(
    bj_bitmap*   p_bitmap,
    usize             width,
    usize             height
) {
    bj_memset(p_bitmap, 0, sizeof(bj_bitmap));
    usize bufsize = width * height;
    if(bufsize > 0) {
        p_bitmap->width = width;
        p_bitmap->height = height;
        if(bufsize > 0) {
            p_bitmap->buffer = bj_malloc(sizeof(bj_color) * bufsize);
        }
        p_bitmap->clear_color = BJ_COLOR_BLACK;
    }
    return p_bitmap;
}


bj_bitmap* bj_bitmap_init_from_file(
    bj_bitmap*   p_bitmap,
    const char*       p_path,
    bj_error*         p_error
) {
    bj_memset(p_bitmap, 0, sizeof(bj_bitmap));
    FILE* bmp_file = fopen(p_path, "rb");
    if (!bmp_file) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_CANNOT_OPEN_FILE);
        return p_bitmap;
    }

    u8 buffer[BMP_INFO_HEADER_SIZE];

    fread(buffer, sizeof(u8), BMP_HEADER_SIZE, bmp_file);
    dib_file_header header;
    dib_read_header(buffer, &header, p_error);
    if(p_error) {
        fclose(bmp_file);
        return p_bitmap;
    }


    fread(buffer, sizeof(u8), BMP_INFO_HEADER_SIZE, bmp_file);
    dib_info_header info_header;
    dib_read_info_header(buffer, &info_header, p_error);
    if(p_error) {
        fclose(bmp_file);
        return p_bitmap;
    }

    usize bufsize = info_header.width * info_header.height;
    if(bufsize == 0) {
        fclose(bmp_file);
        return p_bitmap;
    }
    p_bitmap->width       = info_header.width;
    p_bitmap->height      = info_header.height;
    p_bitmap->buffer      = bj_malloc(sizeof(bj_color) * bufsize);
    p_bitmap->clear_color = BJ_COLOR_BLACK;

    fclose(bmp_file);
    return p_bitmap;
}

// Function to load a BMP file into the bitmap
// TODO remove this
/* BANJO_EXPORT bj_bitmap* bj_bitmap_load_from_bmp( */
/*     bj_bitmap* p_bitmap, */
/*     const char* filename */
/* ) { */
/*     FILE* bmp_file = fopen(filename, "rb"); */
/*     if (!bmp_file) { */
/*         // Failed to open the BMP file */
/*         return NULL; */
/*     } */
    
/*     // Read BMP header */
/*     fseek(bmp_file, 18, SEEK_SET); // Skip to width and height */
/*     uint32_t bmp_width, bmp_height; */
/*     fread(&bmp_width, sizeof(uint32_t), 1, bmp_file); */
/*     fread(&bmp_height, sizeof(uint32_t), 1, bmp_file); */
    
/*     // Allocate memory for the bitmap */
/*     if (!p_bitmap) { */
/*         p_bitmap = bj_malloc(sizeof(bj_bitmap)); */
/*         if (!p_bitmap) { */
/*             // Memory allocation failed */
/*             fclose(bmp_file); */
/*             return NULL; */
/*         } */
/*     } */
    
/*     // Initialize bitmap with BMP dimensions */
/*     bj_bitmap_init_default(p_bitmap, bmp_width, bmp_height); */
    
/*     // Read BMP pixel data */
/*     fseek(bmp_file, 54, SEEK_SET); // Skip header */
/*     for (size_t y = 0; y < bmp_height; y++) { */
/*         for (size_t x = 0; x < bmp_width; x++) { */
/*             uint8_t b, g, r; // BMP stores pixels in BGR order */
/*             fread(&b, sizeof(uint8_t), 1, bmp_file); */
/*             fread(&g, sizeof(uint8_t), 1, bmp_file); */
/*             fread(&r, sizeof(uint8_t), 1, bmp_file); */
            
/*             // Set color in the bitmap */
/*             bj_bitmap_put(p_bitmap, x, y, BJ_RGB(r, g, b)); */
/*         } */
/*         // BMP rows are padded to multiples of 4 bytes */
/*         fseek(bmp_file, (bmp_width * 3) % 4, SEEK_CUR); */
/*     } */
    
/*     fclose(bmp_file); */
/*     return p_bitmap; */
/* } */

BANJO_EXPORT bj_bitmap* bj_bitmap_reset(
    bj_bitmap* p_bitmap
) {
    if(p_bitmap->buffer != 0) {
        bj_free(p_bitmap->buffer);
        p_bitmap->buffer = 0;
    }
    p_bitmap->width       = 0;
    p_bitmap->height      = 0;
    p_bitmap->clear_color = 0;
    return p_bitmap;
}

void bj_bitmap_clear(
    bj_bitmap* p_bitmap
) {
    usize bufsize = p_bitmap->width * p_bitmap->height;
    for(usize i = 0 ; i < bufsize; ++i) {
        p_bitmap->buffer[i] = p_bitmap->clear_color;
    }
}

void bj_bitmap_set_clear_color(
    bj_bitmap* p_bitmap,
    bj_color clear_color
) {
    p_bitmap->clear_color = clear_color;
}

bj_color* bj_bitmap_data(
    bj_bitmap* p_bitmap
) {
    return p_bitmap->buffer;
}

void bj_bitmap_put(
    bj_bitmap* p_bitmap,
    usize x, usize y,
    bj_color color
) {
    put_pixel(p_bitmap->buffer, x, y, color);
}

bj_color bj_bitmap_get(
    bj_bitmap* p_bitmap,
    usize x,
    usize y
) {
    return at(p_bitmap->buffer, x, y);
}

#define X 0
#define Y 1

void bj_bitmap_draw_line(
    bj_bitmap* bmp,
    bj_pixel p0,
    bj_pixel p1,
    bj_color c
) {
    /// Bresenham's line algorithm
    int x0 = p0[X]; int y0 = p0[Y];
    const int x1 = p1[X]; const int y1 = p1[Y];

    const int dx = abs(x1 - x0);
    const int dy = abs(y1 - y0);
    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        bj_bitmap_put(bmp, x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        const int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void bj_bitmap_draw_triangle(
    bj_bitmap* bmp,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    bj_color c
) {
    bj_bitmap_draw_line(bmp, p0, p1, c);
    bj_bitmap_draw_line(bmp, p1, p2, c);
    bj_bitmap_draw_line(bmp, p2, p0, c);
}

