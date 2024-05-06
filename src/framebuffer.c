#include <banjo/error.h>
#include <banjo/framebuffer.h>
#include <banjo/memory.h>

#include <stdlib.h>

#ifdef CENTERED_AXIS
#define S_X(x) ((p_framebuffer->width / 2) + x)
#define S_Y(y) ((p_framebuffer->height / 2) - y)
#else
#define S_X(x) (x)
#define S_Y(y) (y)
#endif
#define XY(x, y) (S_Y(y) * p_framebuffer->width + S_X(x))
#define at(fb, x, y) fb[XY(x, y)]
#define put_pixel(fb, x, y, c) at(fb, x, y) = c

BANJO_EXPORT bj_framebuffer* bj_framebuffer_init_default(
    bj_framebuffer*   p_framebuffer,
    usize             width,
    usize             height
) {
    bj_memset(p_framebuffer, 0, sizeof(bj_framebuffer));
    usize bufsize = width * height;
    if(bufsize > 0) {
        p_framebuffer->width = width;
        p_framebuffer->height = height;
        if(bufsize > 0) {
            p_framebuffer->buffer = bj_malloc(sizeof(bj_color) * bufsize);
        }
        p_framebuffer->clear_color = BJ_COLOR_BLACK;
    }
    return p_framebuffer;
}

BANJO_EXPORT bj_framebuffer* bj_framebuffer_reset(
    bj_framebuffer* p_framebuffer
) {
    if(p_framebuffer->buffer != 0) {
        bj_free(p_framebuffer->buffer);
        p_framebuffer->buffer = 0;
    }
    p_framebuffer->width       = 0;
    p_framebuffer->height      = 0;
    p_framebuffer->clear_color = 0;
    return p_framebuffer;
}

void bj_framebuffer_clear(
    bj_framebuffer* p_framebuffer
) {
    usize bufsize = p_framebuffer->width * p_framebuffer->height;
    for(usize i = 0 ; i < bufsize; ++i) {
        p_framebuffer->buffer[i] = p_framebuffer->clear_color;
    }
}

void bj_framebuffer_set_clear_color(
    bj_framebuffer* p_framebuffer,
    bj_color clear_color
) {
    p_framebuffer->clear_color = clear_color;
}

bj_color* bj_framebuffer_data(
    bj_framebuffer* p_framebuffer
) {
    return p_framebuffer->buffer;
}

void bj_framebuffer_put(
    bj_framebuffer* p_framebuffer,
    usize x, usize y,
    bj_color color
) {
    put_pixel(p_framebuffer->buffer, x, y, color);
}

bj_color bj_framebuffer_get(
    bj_framebuffer* p_framebuffer,
    usize x,
    usize y
) {
    return at(p_framebuffer->buffer, x, y);
}

#define X 0
#define Y 1

void bj_framebuffer_draw_line(
    bj_framebuffer* fb,
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
        bj_framebuffer_put(fb, x0, y0, c);
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

void bj_framebuffer_draw_triangle(
    bj_framebuffer* fb,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    bj_color c
) {
    bj_framebuffer_draw_line(fb, p0, p1, c);
    bj_framebuffer_draw_line(fb, p1, p2, c);
    bj_framebuffer_draw_line(fb, p2, p0, c);
}




