#include <banjo/math.h>
#include <banjo/draw.h>

#include <bitmap.h>
#include <check.h>

#define ABS_INT(x) ((x) < 0 ? -(x) : (x))
#define X 0
#define Y 1

BANJO_EXPORT void bj_draw_line(
    struct bj_bitmap*     bmp,
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
        bj_put_pixel(bmp, x0, y0, pixel);
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

BANJO_EXPORT void bj_draw_rectangle(
    struct bj_bitmap*     p_bitmap,
    const struct bj_rect* p_area,
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
        bj_put_pixel(p_bitmap, x0, y0, pixel);
        return;
    }

    if(horizontal) {
        bj_draw_line(p_bitmap, x0, y0, x1, y0, pixel);
        return;
    }

    if(vertical) {
        bj_draw_line(p_bitmap, x0, y0, x0, y1, pixel);
        return;
    }

    bj_draw_line(p_bitmap, x0, y0, x1, y0, pixel);
    bj_draw_line(p_bitmap, x0, y1, x1, y1, pixel);
    bj_draw_line(p_bitmap, x0, y0+1, x0, y1-1, pixel);
    bj_draw_line(p_bitmap, x1, y0+1, x1, y1-1, pixel);
}

BANJO_EXPORT void bj_draw_filled_rectangle(
    struct bj_bitmap*     p_bitmap,
    const struct bj_rect* p_area,
    uint32_t       pixel
) {
    bj_check(p_bitmap);
    bj_check(p_area);
    for (int x = p_area->x; x < p_area->x + p_area->w; ++x) {
        for (int y = p_area->y; y < p_area->y + p_area->h; ++y) {
            bj_put_pixel(p_bitmap, x, y, pixel);
        }
    }
}

void bj_draw_triangle(
    struct bj_bitmap* bmp,
    int        x0,
    int        y0,
    int        x1,
    int        y1,
    int        x2,
    int        y2,
    uint32_t   c
) {
    bj_draw_line(bmp, x0, y0, x1, y1, c);
    bj_draw_line(bmp, x1, y1, x2, y2, c);
    bj_draw_line(bmp, x2, y2, x0, y0, c);
}

void bj_draw_circle(
    struct bj_bitmap* p_bitmap,
    int        cx,
    int        cy,
    int        radius,
    uint32_t   color)
{
    int r = (int)((bj_real)radius + BJ_F(0.5));
    if (r <= 0) {
        bj_draw_line(p_bitmap, cx, cy, cx, cy, color);
        return;
    }

    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        /* plot 8 symmetric points via 1-pixel lines */
        bj_draw_line(p_bitmap, cx + x, cy + y, cx + x, cy + y, color);
        bj_draw_line(p_bitmap, cx + y, cy + x, cx + y, cy + x, color);
        bj_draw_line(p_bitmap, cx - y, cy + x, cx - y, cy + x, color);
        bj_draw_line(p_bitmap, cx - x, cy + y, cx - x, cy + y, color);
        bj_draw_line(p_bitmap, cx - x, cy - y, cx - x, cy - y, color);
        bj_draw_line(p_bitmap, cx - y, cy - x, cx - y, cy - x, color);
        bj_draw_line(p_bitmap, cx + y, cy - x, cx + y, cy - x, color);
        bj_draw_line(p_bitmap, cx + x, cy - y, cx + x, cy - y, color);

        ++y;
        if (err < 0) {
            err += (y << 1) + 1;
        } else {
            --x;
            err += ((y - x) << 1) + 1;
        }
    }
}

void bj_draw_filled_circle(
    struct bj_bitmap* p_bitmap,
    int        cx,
    int        cy,
    int        radius,
    uint32_t   color)
{
    int r = (int)((bj_real)radius + BJ_F(0.5));
    if (r <= 0) {
        bj_draw_line(p_bitmap, cx, cy, cx, cy, color);
        return;
    }

    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        /* Horizontal spans for the four scanlines of this octant step */
        /* y offsets */
        int y_top    = cy - y;
        int y_bottom = cy + y;
        int y_top2   = cy - x;
        int y_bot2   = cy + x;

        /* spans centered at cx: [cx-x, cx+x] and [cx-y, cx+y] */
        bj_draw_line(p_bitmap, cx - x, y_top,    cx + x, y_top,    color);
        bj_draw_line(p_bitmap, cx - x, y_bottom, cx + x, y_bottom, color);

        /* Avoid redundant spans when x == y (would duplicate the same line) */
        if (x != y) {
            bj_draw_line(p_bitmap, cx - y, y_top2,  cx + y, y_top2,  color);
            bj_draw_line(p_bitmap, cx - y, y_bot2,  cx + y, y_bot2,  color);
        }

        ++y;
        if (err < 0) {
            err += (y << 1) + 1;
        } else {
            --x;
            err += ((y - x) << 1) + 1;
        }
    }
}

void bj_draw_polyline(
    struct bj_bitmap*   bmp,
    size_t       count,
    const int*   x,
    const int*   y,
    bj_bool      loop,
    uint32_t     color
){
    if (!bmp || !x || !y || count <= 0) {
        return;
    }
    if (count == 1) {
        // single point: draw nothing
        return;
    }
    // draw consecutive segments
    for (size_t i = 0; i < count - 1; ++i) {
        bj_draw_line(bmp, x[i], y[i], x[i+1], y[i+1], color);
    }
    if (loop && count >= 2) {
        bj_draw_line(bmp, x[count-1], y[count-1], x[0], y[0], color);
    }
}
