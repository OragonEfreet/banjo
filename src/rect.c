#include <banjo/rect.h>

BANJO_EXPORT bool bj_rect_intersect(
    const bj_rect* p_rect_a,
    const bj_rect* p_rect_b,
    bj_rect* result
) {
    if (!p_rect_a || !p_rect_b) {
        return false;
    }

    // Calculate the left, top, right, and bottom edges of the intersection rectangle
    int x1 = (p_rect_a->x > p_rect_b->x) ? p_rect_a->x : p_rect_b->x;
    int y1 = (p_rect_a->y > p_rect_b->y) ? p_rect_a->y : p_rect_b->y;

    int x2_a = (int)p_rect_a->x + (int)p_rect_a->w;
    int x2_b = (int)p_rect_b->x + (int)p_rect_b->w;
    int y2_a = (int)p_rect_a->y + (int)p_rect_a->h;
    int y2_b = (int)p_rect_b->y + (int)p_rect_b->h;
    
    int x2 = (x2_a < x2_b) ? x2_a : x2_b;
    int y2 = (y2_a < y2_b) ? y2_a : y2_b;

    // Check if the intersection is valid
    if (x2 <= x1 || y2 <= y1) {
        return false; // No intersection
    }

    // Populate the result rectangle with the intersection coordinates and size
    if (result) {
        result->x = (int16_t)x1;
        result->y = (int16_t)y1;
        result->w = (uint16_t)(x2 - x1);
        result->h = (uint16_t)(y2 - y1);
    }

    return true; // Intersection exists
}