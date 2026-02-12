#include <banjo/math.h>
#include <banjo/draw.h>

#include <bitmap.h>
#include <check.h>

#define ABS_INT(x) ((x) < 0 ? -(x) : (x))
#define X 0
#define Y 1

// Fast inline pixel plot with bounds check.
// Avoids the overhead of bj_put_pixel's format dispatch in the hot loop
// by checking bounds once and using format-specific direct writes.
static inline void plot_pixel_fast(
    struct bj_bitmap* bmp,
    int px, int py,
    uint32_t color,
    int w, int h,
    size_t bpp
) {
    // Bounds check - single branch for the common in-bounds case
    if ((unsigned)px >= (unsigned)w || (unsigned)py >= (unsigned)h) {
        return;
    }

    uint8_t* row = bj_row_ptr(bmp, (size_t)py);

    switch (bpp) {
    case 32:
        bj_put_pixel_32(row, (size_t)px, color);
        break;
    case 24:
        bj_put_pixel_24(row, (size_t)px, color);
        break;
    case 16:
        bj_put_pixel_16(row, (size_t)px, (uint16_t)color);
        break;
    case 8:
        bj_put_pixel_8(row, (size_t)px, (uint8_t)color);
        break;
    default:
        // Sub-byte formats: fall back to validated API
        bj_put_pixel(bmp, (size_t)px, (size_t)py, color);
        break;
    }
}

BANJO_EXPORT void bj_draw_line(
    struct bj_bitmap*     bmp,
    int            x0,
    int            y0,
    int            x1,
    int            y1,
    uint32_t       pixel
) {
    bj_check(bmp);

    // Cache dimensions and BPP once - avoids per-pixel struct access
    const int w = (int)bmp->width;
    const int h = (int)bmp->height;
    const size_t bpp = BJ_PIXEL_GET_BPP(bmp->mode);

    // Bresenham's line algorithm with optimized pixel writes
    const int dx = ABS_INT(x1 - x0);
    const int dy = ABS_INT(y1 - y0);
    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        plot_pixel_fast(bmp, x0, y0, pixel, w, h, bpp);
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

// Fast vertical line with direct pixel access.
// Uses pre-computed BPP to avoid per-pixel format dispatch.
// Row pointer is computed once and incremented by stride (not multiplied per-pixel).
static inline void vline_fast(
    struct bj_bitmap* bmp,
    int x, int y0, int y1,
    uint32_t color,
    size_t bpp
) {
    // Ensure y0 <= y1
    if (y0 > y1) {
        int tmp = y0; y0 = y1; y1 = tmp;
    }

    // Clip to bitmap bounds
    const int w = (int)bmp->width;
    const int h = (int)bmp->height;
    if (x < 0 || x >= w) return;
    if (y0 < 0) y0 = 0;
    if (y1 >= h) y1 = h - 1;
    if (y0 > y1) return;

    // Compute row pointer once, then increment by stride (avoids y*stride per pixel)
    uint8_t* row = bj_row_ptr(bmp, (size_t)y0);
    const size_t stride = bmp->stride;
    const int count = y1 - y0 + 1;

    switch (bpp) {
    case 32:
        for (int i = 0; i < count; ++i) {
            bj_put_pixel_32(row, (size_t)x, color);
            row += stride;
        }
        break;
    case 24:
        for (int i = 0; i < count; ++i) {
            bj_put_pixel_24(row, (size_t)x, color);
            row += stride;
        }
        break;
    case 16:
        for (int i = 0; i < count; ++i) {
            bj_put_pixel_16(row, (size_t)x, (uint16_t)color);
            row += stride;
        }
        break;
    case 8:
        for (int i = 0; i < count; ++i) {
            bj_put_pixel_8(row, (size_t)x, (uint8_t)color);
            row += stride;
        }
        break;
    default:
        // Sub-byte formats: fall back to validated API
        for (int y = y0; y <= y1; ++y) {
            bj_put_pixel(bmp, (size_t)x, (size_t)y, color);
        }
        break;
    }
}

// Fast horizontal line dispatcher (local helper for rectangle).
// Wraps the format-specific hline functions.
static inline void hline_rect(
    struct bj_bitmap* bmp,
    int x0, int x1, int y,
    uint32_t color,
    size_t bpp
) {
    // Ensure x0 <= x1
    if (x0 > x1) {
        int tmp = x0; x0 = x1; x1 = tmp;
    }

    switch (bpp) {
    case 32:
        bj_hline_32(bmp, x0, x1 + 1, y, color);
        break;
    case 24:
        bj_hline_24(bmp, x0, x1 + 1, y, color);
        break;
    case 16:
        bj_hline_16(bmp, x0, x1 + 1, y, color);
        break;
    default:
        bj_hline_generic(bmp, x0, x1 + 1, y, color);
        break;
    }
}

BANJO_EXPORT void bj_draw_rectangle(
    struct bj_bitmap*     p_bitmap,
    const struct bj_rect* p_area,
    uint32_t       pixel
) {
    bj_check(p_bitmap);
    bj_check(p_area);

    const int x0 = p_area->x;
    const int y0 = p_area->y;
    const int x1 = x0 + p_area->w;
    const int y1 = y0 + p_area->h;

    // Cache BPP once for all edges
    const size_t bpp = BJ_PIXEL_GET_BPP(p_bitmap->mode);

    const bj_bool horizontal = y0 == y1;
    const bj_bool vertical = x0 == x1;

    if (horizontal && vertical) {
        // Single pixel - use fast path
        const int w = (int)p_bitmap->width;
        const int h = (int)p_bitmap->height;
        if ((unsigned)x0 < (unsigned)w && (unsigned)y0 < (unsigned)h) {
            uint8_t* row = bj_row_ptr(p_bitmap, (size_t)y0);
            switch (bpp) {
            case 32: bj_put_pixel_32(row, (size_t)x0, pixel); break;
            case 24: bj_put_pixel_24(row, (size_t)x0, pixel); break;
            case 16: bj_put_pixel_16(row, (size_t)x0, (uint16_t)pixel); break;
            case 8:  bj_put_pixel_8(row, (size_t)x0, (uint8_t)pixel); break;
            default: bj_put_pixel(p_bitmap, (size_t)x0, (size_t)y0, pixel); break;
            }
        }
        return;
    }

    if (horizontal) {
        hline_rect(p_bitmap, x0, x1, y0, pixel, bpp);
        return;
    }

    if (vertical) {
        vline_fast(p_bitmap, x0, y0, y1, pixel, bpp);
        return;
    }

    // Full rectangle: 2 horizontal + 2 vertical edges
    hline_rect(p_bitmap, x0, x1, y0, pixel, bpp);      // Top edge
    hline_rect(p_bitmap, x0, x1, y1, pixel, bpp);      // Bottom edge
    vline_fast(p_bitmap, x0, y0 + 1, y1 - 1, pixel, bpp);  // Left edge (excluding corners)
    vline_fast(p_bitmap, x1, y0 + 1, y1 - 1, pixel, bpp);  // Right edge (excluding corners)
}

BANJO_EXPORT void bj_draw_filled_rectangle(
    struct bj_bitmap*     p_bitmap,
    const struct bj_rect* p_area,
    uint32_t       pixel
) {
    bj_check(p_bitmap);
    bj_check(p_area);

    const int x0 = p_area->x;
    const int y0 = p_area->y;
    const int x1 = x0 + p_area->w;
    const int y1 = y0 + p_area->h;

    // Dispatch to format-specific fill for maximum speed
    const size_t bpp = BJ_PIXEL_GET_BPP(p_bitmap->mode);
    switch (bpp) {
    case 32:
        bj_fill_rect_32(p_bitmap, x0, y0, x1, y1, pixel);
        break;
    case 24:
        bj_fill_rect_24(p_bitmap, x0, y0, x1, y1, pixel);
        break;
    case 16:
        bj_fill_rect_16(p_bitmap, x0, y0, x1, y1, pixel);
        break;
    default:
        bj_fill_rect_generic(p_bitmap, x0, y0, x1, y1, pixel);
        break;
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
    uint32_t   color
) {
    bj_draw_line(bmp, x0, y0, x1, y1, color);
    bj_draw_line(bmp, x1, y1, x2, y2, color);
    bj_draw_line(bmp, x2, y2, x0, y0, color);
}

#define SWAP_INT(a, b) do { int tmp = a; a = b; b = tmp; } while(0)
#define SWAP_COORDS(x0, y0, x1, y1) do { SWAP_INT(x0, x1); SWAP_INT(y0, y1); } while(0)

// Fast horizontal line dispatcher.
// Selects format-specific hline or falls back to generic.
// The bj_hline_* functions handle clipping internally.
static inline void hline_fast(
    struct bj_bitmap* bmp,
    int x0, int x1, int y,
    uint32_t color,
    size_t bpp
) {
    // Ensure x0 <= x1
    if (x0 > x1) {
        int tmp = x0; x0 = x1; x1 = tmp;
    }

    switch (bpp) {
    case 32:
        bj_hline_32(bmp, x0, x1 + 1, y, color);  // +1: hline uses exclusive end
        break;
    case 24:
        bj_hline_24(bmp, x0, x1 + 1, y, color);
        break;
    case 16:
        bj_hline_16(bmp, x0, x1 + 1, y, color);
        break;
    default:
        bj_hline_generic(bmp, x0, x1 + 1, y, color);
        break;
    }
}

void bj_draw_filled_triangle(
    struct bj_bitmap* bmp,
    int        x0,
    int        y0,
    int        x1,
    int        y1,
    int        x2,
    int        y2,
    uint32_t   color
) {
    bj_check(bmp);

    // Sort vertices by Y coordinate: p0 < p1 < p2
    if (y1 < y0) { SWAP_COORDS(x0, y0, x1, y1); }
    if (y2 < y0) { SWAP_COORDS(x0, y0, x2, y2); }
    if (y2 < y1) { SWAP_COORDS(x1, y1, x2, y2); }

    // Cache BPP for hline_fast dispatch
    const size_t bpp = BJ_PIXEL_GET_BPP(bmp->mode);

    // Degenerate case: all points on same horizontal line
    if (y0 == y2) {
        int min_x = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
        int max_x = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
        hline_fast(bmp, min_x, max_x, y0, color, bpp);
        return;
    }

    // Interpolation for the long edge (p0 to p2)
    const float x02_a = (float)(x2 - x0) / (float)(y2 - y0);
    float x02_d = (float)x0;

    // First triangle half (top): from p0 to p1
    if (y1 > y0) {
        const float x01_a = (float)(x1 - x0) / (float)(y1 - y0);
        float x01_d = (float)x0;

        for (int y = y0; y < y1; ++y) {
            int left_x = (int)x01_d;
            int right_x = (int)x02_d;

            if (right_x < left_x) {
                SWAP_INT(left_x, right_x);
            }

            hline_fast(bmp, left_x, right_x, y, color, bpp);

            x01_d += x01_a;
            x02_d += x02_a;
        }
    }

    // Second triangle half (bottom): from p1 to p2
    if (y2 > y1) {
        const float x12_a = (float)(x2 - x1) / (float)(y2 - y1);
        float x12_d = (float)x1;

        for (int y = y1; y <= y2; ++y) {
            int left_x = (int)x12_d;
            int right_x = (int)x02_d;

            if (right_x < left_x) {
                SWAP_INT(left_x, right_x);
            }

            hline_fast(bmp, left_x, right_x, y, color, bpp);

            x12_d += x12_a;
            x02_d += x02_a;
        }
    }
}

#undef SWAP_COORDS
#undef SWAP_INT

void bj_draw_circle(
    struct bj_bitmap* p_bitmap,
    int        cx,
    int        cy,
    int        radius,
    uint32_t   color)
{
    bj_check(p_bitmap);

    int r = (int)((bj_real)radius + BJ_F(0.5));
    if (r <= 0) {
        // Degenerate: single pixel
        if ((unsigned)cx < p_bitmap->width && (unsigned)cy < p_bitmap->height) {
            bj_put_pixel(p_bitmap, (size_t)cx, (size_t)cy, color);
        }
        return;
    }

    // Cache bitmap dimensions and BPP to avoid repeated struct access
    const int w = (int)p_bitmap->width;
    const int h = (int)p_bitmap->height;
    const size_t bpp = BJ_PIXEL_GET_BPP(p_bitmap->mode);

    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        // Plot 8 symmetric points using direct pixel writes
        plot_pixel_fast(p_bitmap, cx + x, cy + y, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx + y, cy + x, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx - y, cy + x, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx - x, cy + y, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx - x, cy - y, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx - y, cy - x, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx + y, cy - x, color, w, h, bpp);
        plot_pixel_fast(p_bitmap, cx + x, cy - y, color, w, h, bpp);

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
    bj_check(p_bitmap);

    int r = (int)((bj_real)radius + BJ_F(0.5));
    if (r <= 0) {
        // Degenerate: single pixel
        if ((unsigned)cx < p_bitmap->width && (unsigned)cy < p_bitmap->height) {
            bj_put_pixel(p_bitmap, (size_t)cx, (size_t)cy, color);
        }
        return;
    }

    // Cache BPP to avoid repeated extraction in the loop
    const size_t bpp = BJ_PIXEL_GET_BPP(p_bitmap->mode);

    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        // Horizontal spans for the four scanlines of this octant step
        int y_top    = cy - y;
        int y_bottom = cy + y;
        int y_top2   = cy - x;
        int y_bot2   = cy + x;

        // Spans centered at cx: [cx-x, cx+x] and [cx-y, cx+y]
        hline_fast(p_bitmap, cx - x, cx + x, y_top,    color, bpp);
        hline_fast(p_bitmap, cx - x, cx + x, y_bottom, color, bpp);

        // Avoid redundant spans when x == y (would duplicate the same line)
        if (x != y) {
            hline_fast(p_bitmap, cx - y, cx + y, y_top2, color, bpp);
            hline_fast(p_bitmap, cx - y, cx + y, y_bot2, color, bpp);
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
