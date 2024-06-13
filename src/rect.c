#include <banjo/rect.h>

BANJO_EXPORT int bj_rect_intersect(
    const bj_rect* p_rect_a,
    const bj_rect* p_rect_b,
    bj_rect* result
) {
    // Calculate the left, top, right, and bottom edges of the intersection rectangle
    int x1 = (p_rect_a->x > p_rect_b->x) ? p_rect_a->x : p_rect_b->x;
    int y1 = (p_rect_a->y > p_rect_b->y) ? p_rect_a->y : p_rect_b->y;
    int x2 = (p_rect_a->x + p_rect_a->w < p_rect_b->x + p_rect_b->w) ? p_rect_a->x + p_rect_a->w : p_rect_b->x + p_rect_b->w;
    int y2 = (p_rect_a->y + p_rect_a->h < p_rect_b->y + p_rect_b->h) ? p_rect_a->y + p_rect_a->h : p_rect_b->y + p_rect_b->h;

    // Check if the intersection is valid
    if (x2 <= x1 || y2 <= y1) {
        return false; // No intersection
    }

    // Populate the result rectangle with the intersection coordinates and size
    if(result) {
        result->x = x1;
        result->y = y1;
        result->w = x2 - x1;
        result->h = y2 - y1;
    }

    return true; // Intersection exists
}
