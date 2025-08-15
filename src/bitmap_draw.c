#include <banjo/math.h>
#include <banjo/draw.h>

#include "bitmap_t.h"
#include "check.h"

#define ABS_INT(x) ((x) < 0 ? -(x) : (x))
#define X 0
#define Y 1

BANJO_EXPORT void bj_bitmap_draw_line(
    bj_bitmap*     bmp,
    int            x0,
    int            y0,
    int            x1,
    int            y1,
    uint32_t       pixel
) {
    /// Bresenham's line algorithm
    const int dx = ABS_INT(x1 - x0);
    const int dy = ABS_INT(y1 - y0);
    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        bj_bitmap_put_pixel(bmp, x0, y0, pixel);
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

BANJO_EXPORT void bj_bitmap_draw_rectangle(
    bj_bitmap*     p_bitmap,
    const bj_rect* p_area,
    uint32_t       pixel
) {
    bj_check(p_area);

    const int x0 = p_area->x;
    const int y0 = p_area->y;
    const int x1 = x0 + p_area->w;
    const int y1 = y0 + p_area->h;

    const bj_bool horizontal = y0 == y1;
    const bj_bool vertical = x0 == x1;

    if(horizontal && vertical) {
        bj_bitmap_put_pixel(p_bitmap, x0, y0, pixel);
        return;
    }

    if(horizontal) {
        bj_bitmap_draw_line(p_bitmap, x0, y0, x1, y0, pixel);
        return;
    }

    if(vertical) {
        bj_bitmap_draw_line(p_bitmap, x0, y0, x0, y1, pixel);
        return;
    }

    bj_bitmap_draw_line(p_bitmap, x0, y0, x1, y0, pixel);
    bj_bitmap_draw_line(p_bitmap, x0, y1, x1, y1, pixel);
    bj_bitmap_draw_line(p_bitmap, x0, y0+1, x0, y1-1, pixel);
    bj_bitmap_draw_line(p_bitmap, x1, y0+1, x1, y1-1, pixel);
}

BANJO_EXPORT void bj_bitmap_draw_filled_rectangle(
    bj_bitmap*     p_bitmap,
    const bj_rect* p_area,
    uint32_t       pixel
) {
    bj_check(p_bitmap);
    bj_check(p_area);
    for (int x = p_area->x; x < p_area->x + p_area->w; ++x) {
        for (int y = p_area->y; y < p_area->y + p_area->h; ++y) {
            bj_bitmap_put_pixel(p_bitmap, x, y, pixel);
        }
    }
}

void bj_bitmap_draw_triangle(
    bj_bitmap* bmp,
    int        x0,
    int        y0,
    int        x1,
    int        y1,
    int        x2,
    int        y2,
    uint32_t   c
) {
    bj_bitmap_draw_line(bmp, x0, y0, x1, y1, c);
    bj_bitmap_draw_line(bmp, x1, y1, x2, y2, c);
    bj_bitmap_draw_line(bmp, x2, y2, x0, y0, c);
}
