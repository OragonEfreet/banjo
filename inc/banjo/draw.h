////////////////////////////////////////////////////////////////////////////////
/// \file draw.h
/// Header file for \ref bitmap drawing functions.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup drawing Drawing
/// \ingroup bitmap
///
/// \brief 2D drawing facilities
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_DRAW_H
#define BJ_DRAW_H
#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>

////////////////////////////////////////////////////////////////////////////////
/// Draws a line of pixels in the given bitmap.
///
/// The line is drawn for each pixel between p0 and p1.
///
/// \param bitmap The bitmap object.
/// \param x0       The X coordinate of the first point in the line.
/// \param y0       The Y coordinate of the first point in the line.
/// \param x1       The X coordinate of the second point in the line.
/// \param y1       The Y coordinate of the second point in the line.
/// \param pixel    The line pixel value.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, struct bj_bitmap->width * struct bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_line(
    struct bj_bitmap*     bitmap,
    int            x0,
    int            y0,
    int            x1,
    int            y1,
    uint32_t       pixel
);

////////////////////////////////////////////////////////////////////////////////
/// Draws a rectangle in the given bitmap
///
/// The function draws an rectangle outline using 4 consecutive calls to
/// \ref bj_draw_line.
///
/// \param bitmap The bitmap object.
/// \param area   The rectangle to draw.
/// \param pixel    The line pixel value.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, struct bj_bitmap->width * struct bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_rectangle(
    struct bj_bitmap*     bitmap,
    const struct bj_rect* area,
    uint32_t       pixel
);

////////////////////////////////////////////////////////////////////////////////
/// Draws a filled rectangle in the given bitmap
///
/// The function draws an filled rectangle by filling all pixels within 
/// `area`.
///
/// \param bitmap The bitmap object.
/// \param area   The rectangle to draw.
/// \param pixel    The line pixel value.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, struct bj_bitmap->width * struct bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_filled_rectangle(
    struct bj_bitmap*     bitmap,
    const struct bj_rect* area,
    uint32_t       pixel
);

////////////////////////////////////////////////////////////////////////////////
/// Draws the edges of a triangle given its 3 corners.
///
/// \param bitmap The bitmap object.
/// \param x0       The X coordinate of the first triangle vertex.
/// \param y0       The Y coordinate of the first triangle vertex.
/// \param x1       The X coordinate of the second triangle vertex.
/// \param y1       The Y coordinate of the second triangle vertex.
/// \param x2       The X coordinate of the third triangle vertex.
/// \param y2       The Y coordinate of the third triangle vertex.
/// \param color    The line color.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, struct bj_bitmap->width * struct bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_triangle(
    struct bj_bitmap* bitmap,
    int        x0,
    int        y0,
    int        x1,
    int        y1,
    int        x2,
    int        y2,
    uint32_t   color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw the outline of a circle onto a bitmap.
///
/// Uses the midpoint circle algorithm (integer arithmetic).
///
/// \param bitmap Target bitmap (must not be NULL).
/// \param cx       X-coordinate of circle center (pixels).
/// \param cy       Y-coordinate of circle center (pixels).
/// \param radius   Circle radius in pixels (>= 0).
/// \param color    Pixel color in 0xAARRGGBB format.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_circle(
    struct bj_bitmap* bitmap,
    int        cx,
    int        cy,
    int        radius,
    uint32_t   color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw a filled circle onto a bitmap.
///
/// Fills all pixels within radius distance from (cx, cy).
///
/// \param bitmap Target bitmap (must not be NULL).
/// \param cx       X-coordinate of circle center (pixels).
/// \param cy       Y-coordinate of circle center (pixels).
/// \param radius   Circle radius in pixels (>= 0).
/// \param color    Pixel color in 0xAARRGGBB format.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_filled_circle(
    struct bj_bitmap* bitmap,
    int        cx,
    int        cy,
    int        radius,
    uint32_t   color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw a polyline from C-style coordinate arrays.
///
/// Draws line segments between successive vertex pairs (x[i], y[i]) -> (x[i+1], y[i+1]).
/// If loop != 0 and count >= 2, an extra segment connects the last vertex to the first.
///
/// \param bitmap Target bitmap.
/// \param x        Pointer to array of x coordinates (length >= count).
/// \param y        Pointer to array of y coordinates (length >= count).
/// \param count    Number of vertices. Segments drawn for i = 0..count-2.
/// \param color    Pixel color in 0xAARRGGBB format.
/// \param loop     Nonzero to close the polyline.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_polyline(
    struct bj_bitmap*   bitmap,
    size_t       count,
    const int*   x,
    const int*   y,
    bj_bool      loop,
    uint32_t     color
);


#endif
/// \} // End of drawing group
