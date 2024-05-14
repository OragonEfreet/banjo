#include <banjo/error.h>
#include <banjo/bitmap.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "dib.h"

#include <stdlib.h>
#include <stdio.h>

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

#define BJ_DIB_INFO_HEADER_SIZE 40

bj_bitmap* bj_bitmap_init_from_file(
    bj_bitmap*   p_bitmap,
    const char*  p_path,
    bj_error*    p_error
) {
    bj_memset(p_bitmap, 0, sizeof(bj_bitmap));

    FILE* fstream  = fopen(p_path, "rb");
    if (!fstream ) {
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_CANNOT_OPEN_FILE);
        return p_bitmap;
    }

    // Load header onto memory
    u8* buffer = bj_malloc(BJ_DIB_INFO_HEADER_SIZE); // Allocate to header size to avoid 1 allocation
    fread(buffer, 1, BJ_DIB_HEADER_SIZE, fstream); 

    dib_file_header file_header;
    dib_read_file_header(&file_header, buffer, p_error);

    // Load Info Header
    fread(buffer, 1, BJ_DIB_INFO_HEADER_SIZE, fstream);
    dib_info_header info_header;
    dib_read_info_header(&info_header, buffer, p_error);

    // Load Color table
    usize n_colors = dib_color_table_size(info_header.bit_count);
    usize table_color_size = (sizeof(u8)*4) * n_colors;

    buffer = bj_realloc(buffer, table_color_size);
    fread(buffer, 1, table_color_size, fstream);

#ifdef BANJO_PEDANTIC
    if(ftell(fstream) != file_header.data_offset) {
        bj_free(buffer);
        bj_set_error(p_error, BJ_DOMAIN_IO, BJ_CANNOT_OPEN_FILE);
        return p_bitmap;
    }
#endif

    bj_array* p_color_table = bj_new(array, default_t, table_color);
    dib_read_color_table(p_color_table, buffer, n_colors, p_error);

    p_bitmap = bj_bitmap_init_default(p_bitmap, info_header.width, info_header.height);
    bj_bitmap_set_clear_color(p_bitmap, BJ_COLOR_BLACK);

    fseek(fstream, file_header.data_offset, SEEK_SET);
    dib_read_raster(p_bitmap, &info_header, p_color_table, p_error);

    bj_del(array, p_color_table);
    bj_free(buffer);


    return p_bitmap;
}

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

