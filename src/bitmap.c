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

bj_bitmap* bj_bitmap_init_from_file(
    bj_bitmap*   p_bitmap,
    const char*  p_path,
    bj_error**   p_error
) {
    bj_check_or_0(p_bitmap);
    bj_memset(p_bitmap, 0, sizeof(bj_bitmap));

    FILE* fstream  = fopen(p_path, "rb");
    if (!fstream ) {
        bj_set_error(p_error, BJ_ERROR_FILE_NOT_FOUND, "Cannot open BMP file");
        return p_bitmap;
    }

    u8* buffer = bj_malloc(BJ_DIB_HEADER_SIZE);
    if(buffer == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot allocate buffer");
        bj_free(buffer);
        fclose(fstream);
        return p_bitmap;
    }

    size_t bytes_read = fread(buffer, 1, BJ_DIB_HEADER_SIZE, fstream);
    if(bytes_read != BJ_DIB_HEADER_SIZE) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "BMP File does not meet expected size");
        bj_free(buffer);
        fclose(fstream);
        return p_bitmap;
    }

    bj_error* p_inner_error = 0;

    dib_file_header file_header;
    dib_read_file_header(&file_header, buffer, &p_inner_error);
    if(p_inner_error) {
        bj_free(buffer);
        fclose(fstream);
        bj_forward_error(p_inner_error, p_error);
        return p_bitmap;
    }
    bj_free(buffer);

    const usize dib_size = file_header.file_size - BJ_DIB_HEADER_SIZE;
    buffer = bj_malloc(dib_size); 
    if(buffer == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot allocate buffer");
        fclose(fstream);
        return p_bitmap;
    }

    bytes_read = fread(buffer, 1, dib_size, fstream);
    fclose(fstream);
    if(bytes_read != dib_size) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "BMP File does not meet expected size");
        bj_free(buffer);
        return p_bitmap;
    }

    dib_read_bitmap(p_bitmap, buffer, dib_size, file_header.data_offset - BJ_DIB_HEADER_SIZE, &p_inner_error);
    bj_free(buffer);
    if(p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
    }

    return p_bitmap;
}

BANJO_EXPORT bj_bitmap* bj_bitmap_reset(
    bj_bitmap* p_bitmap
) {
    if(p_bitmap != 0) {
        if(p_bitmap->buffer != 0) {
            bj_free(p_bitmap->buffer);
            p_bitmap->buffer = 0;
        }
        p_bitmap->width       = 0;
        p_bitmap->height      = 0;
        p_bitmap->clear_color = 0;
    }
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

