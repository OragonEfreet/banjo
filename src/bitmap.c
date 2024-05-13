#include <banjo/error.h>
#include <banjo/bitmap.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/dib.h>

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
    const char*  p_path,
    bj_error*    p_error
) {
    bj_memset(p_bitmap, 0, sizeof(bj_bitmap));

    bj_dib* p_dib = bj_new(dib, from_file, p_path, 0);

    p_bitmap->width       = p_dib->info_header.width;
    p_bitmap->height      = p_dib->info_header.height;
    /* p_bitmap->buffer      = bj_malloc(sizeof(bj_color) * bufsize); */
    p_bitmap->clear_color = BJ_COLOR_BLACK;

    bj_del(dib, p_dib);

    /* bj_dib bj_dib_file; */
    /* bj_dib_read_file(p_path, &bj_dib_file, p_error); */





    /* u8 buffer[BJ_DIB_INFO_HEADER_SIZE]; */

    /* fread(buffer, sizeof(u8), BJ_DIB_HEADER_SIZE, bmp_file); */
    /* bj_dib_file_header header; */
    /* bj_dib_read_header(buffer, &header, p_error); */
    /* if(p_error) { */
    /*     fclose(bmp_file); */
    /*     return p_bitmap; */
    /* } */

    /* fread(buffer, sizeof(u8), BJ_DIB_INFO_HEADER_SIZE, bmp_file); */
    /* bj_dib_info_header info_header; */
    /* bj_dib_read_info_header(buffer, &info_header, p_error); */
    /* if(p_error) { */
    /*     fclose(bmp_file); */
    /*     return p_bitmap; */
    /* } */

    /* usize bufsize = info_header.width * info_header.height; */
    /* if(bufsize == 0) { */
    /*     fclose(bmp_file); */
    /*     return p_bitmap; */
    /* } */

    /* bj_array* p_color_table = bj_new(array, default_t, bj_dib_table_color); */ 
    /* bj_dib_read_color_table( */





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

