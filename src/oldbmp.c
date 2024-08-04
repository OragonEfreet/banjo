#include <banjo/error.h>
#include <banjo/oldbmp.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "oldbmp_t.h"
#include "dib.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef CENTERED_AXIS
#define S_X(x) ((p_oldbmp->width / 2) + x)
#define S_Y(y) ((p_oldbmp->height / 2) - y)
#else
#define S_X(x) (x)
#define S_Y(y) (y)
#endif
#define XY(x, y) (S_Y(y) * p_oldbmp->width + S_X(x))
#define at(bmp, x, y) bmp[XY(x, y)]
#define put_pixel(bmp, x, y, c) at(bmp, x, y) = c

bj_oldbmp* bj_oldbmp_init(
    bj_oldbmp* p_oldbmp,
    usize      width,
    usize      height
) {
    if(p_oldbmp) {
        bj_memset(p_oldbmp, 0, sizeof(bj_oldbmp));
        usize bufsize = width * height;
        if(bufsize == 0) {
            return 0;
        }

        p_oldbmp->width = width;
        p_oldbmp->height = height;
        p_oldbmp->buffer = bj_malloc(sizeof(bj_color) * bufsize);
        p_oldbmp->clear_color = BJ_COLOR_BLACK;
        bj_oldbmp_clear(p_oldbmp);
    }
    return p_oldbmp;
}

void bj_oldbmp_reset(
    bj_oldbmp* p_oldbmp
) {
    bj_check(p_oldbmp);

    if(p_oldbmp->buffer != 0) {
        bj_free(p_oldbmp->buffer);
    }
#ifdef BJ_FEAT_PEDANTIC
    bj_memset(p_oldbmp, 0, sizeof(bj_oldbmp));
#endif
}

BANJO_EXPORT bj_oldbmp* bj_oldbmp_new(
    usize        width,
    usize        height
) {
    bj_oldbmp oldbmp;
    if (bj_oldbmp_init(&oldbmp, width, height) == 0) {
        return 0;
    }
    return bj_memcpy(bj_malloc(sizeof(bj_oldbmp)), &oldbmp, sizeof(oldbmp));
}

bj_oldbmp* bj_oldbmp_new_from_file(
    const char*  p_path,
    bj_error**   p_error
) {
    FILE* fstream  = fopen(p_path, "rb");
    if (!fstream ) {
        bj_set_error(p_error, BJ_ERROR_FILE_NOT_FOUND, "Cannot open BMP file");
        return 0;
    }

    u8* buffer = bj_malloc(BJ_DIB_HEADER_SIZE);
    if(buffer == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot allocate buffer");
        bj_free(buffer);
        fclose(fstream);
        return 0;
    }

    size_t bytes_read = fread(buffer, 1, BJ_DIB_HEADER_SIZE, fstream);
    if(bytes_read != BJ_DIB_HEADER_SIZE) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "BMP File does not meet expected size");
        bj_free(buffer);
        fclose(fstream);
        return 0;
    }

    bj_error* p_inner_error = 0;

    dib_file_header file_header;
    dib_read_file_header(&file_header, buffer, &p_inner_error);
    if(p_inner_error) {
        bj_free(buffer);
        fclose(fstream);
        bj_forward_error(p_inner_error, p_error);
        return 0;
    }
    bj_free(buffer);

    const usize dib_size = file_header.file_size - BJ_DIB_HEADER_SIZE;
    buffer = bj_malloc(dib_size); 
    if(buffer == 0) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "Cannot allocate buffer");
        fclose(fstream);
        return 0;
    }

    bytes_read = fread(buffer, 1, dib_size, fstream);
    fclose(fstream);
    if(bytes_read != dib_size) {
        bj_set_error(p_error, BJ_ERROR_INVALID_FORMAT, "BMP File does not meet expected size");
        bj_free(buffer);
        return 0;
    }

    bj_oldbmp oldbmp;
    dib_read_oldbmp(&oldbmp, buffer, dib_size, file_header.data_offset - BJ_DIB_HEADER_SIZE, &p_inner_error);

    bj_free(buffer);
    if(p_inner_error) {
        bj_forward_error(p_inner_error, p_error);
        return 0;
    }

    return bj_memcpy(bj_malloc(sizeof(bj_oldbmp)), &oldbmp, sizeof(bj_oldbmp));
}

BANJO_EXPORT void bj_oldbmp_del(
    bj_oldbmp* p_oldbmp
) {
    bj_oldbmp_reset(p_oldbmp);
    bj_free(p_oldbmp);
}

void bj_oldbmp_clear(
    bj_oldbmp* p_oldbmp
) {
    usize bufsize = p_oldbmp->width * p_oldbmp->height;
    for(usize i = 0 ; i < bufsize; ++i) {
        p_oldbmp->buffer[i] = p_oldbmp->clear_color;
    }
}

void bj_oldbmp_set_clear_color(
    bj_oldbmp* p_oldbmp,
    bj_color clear_color
) {
    p_oldbmp->clear_color = clear_color;
}

bj_color* bj_oldbmp_data(
    bj_oldbmp* p_oldbmp
) {
    return p_oldbmp->buffer;
}

void bj_oldbmp_put(
    bj_oldbmp* p_oldbmp,
    usize x, usize y,
    bj_color color
) {
    put_pixel(p_oldbmp->buffer, x, y, color);
}

bj_color bj_oldbmp_get(
    const bj_oldbmp* p_oldbmp,
    usize            x,
    usize            y
) {
    return at(p_oldbmp->buffer, x, y);
}

#define X 0
#define Y 1

void bj_oldbmp_draw_line(
    bj_oldbmp* bmp,
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
        bj_oldbmp_put(bmp, x0, y0, c);
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

void bj_oldbmp_draw_triangle(
    bj_oldbmp* bmp,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    bj_color c
) {
    bj_oldbmp_draw_line(bmp, p0, p1, c);
    bj_oldbmp_draw_line(bmp, p1, p2, c);
    bj_oldbmp_draw_line(bmp, p2, p0, c);
}

bool bj_oldbmp_blit(
    const bj_oldbmp* p_src,
    const bj_rect*   p_src_rect,
    bj_oldbmp*       p_dest,
    bj_rect*         p_dest_rect
) {

    // Clip the source rect
    bj_rect blit_rect;
    if( bj_rect_intersect(
        &(bj_rect){.w = p_src->width, .h = p_src->width, },
        p_src_rect, &blit_rect) == true
    ) {
        p_dest_rect->w = blit_rect.w;
        p_dest_rect->h = blit_rect.h;

        if( bj_rect_intersect(
            &(bj_rect) {.w = p_dest->width, .h = p_dest->height, },
            p_dest_rect, p_dest_rect) == true
        ) {
            for(usize r = 0 ; r < p_dest_rect->h ; ++r) {
                const usize from_y = blit_rect.y + r;
                const usize to_y = p_dest_rect->y + r;
                for(usize c = 0 ; c < p_dest_rect->w ; ++c) {
                    bj_oldbmp_put(p_dest,
                        p_dest_rect->x + c,
                        to_y,
                        bj_oldbmp_get(p_src, blit_rect.x + c, from_y)
                    );
                }
            }
        }

        return true;
    }

    return false;
}
