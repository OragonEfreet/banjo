////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref bitmap drawing functions.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup drawing Drawing
/// \ingroup graphics
///
/// \brief 2D drawing facilities
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>

/// Represents a pixel position in a bitmap.
typedef int bj_pixel[2];

////////////////////////////////////////////////////////////////////////////////
/// Draws a line of pixels in the given bitmap.
///
/// The line is drawn for each pixel between p0 and p1.
///
/// \param p_bitmap The bitmap object.
/// \param p0       The first point in the line.
/// \param p1       The second point in the line.
/// \param pixel    The line pixel value.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_bitmap->width * bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_draw_line(
    bj_bitmap*     p_bitmap,
    bj_pixel       p0,
    bj_pixel       p1,
    uint32_t       pixel
);

////////////////////////////////////////////////////////////////////////////////
/// Draws a rectangle in the given bitmap
///
/// The function draws an rectangle outline using 4 consecutive calls to
/// \ref bj_bitmap_draw_line.
///
/// \param p_bitmap The bitmap object.
/// \param p_area   The rectangle to draw.
/// \param pixel    The line pixel value.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_bitmap->width * bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_draw_rectangle(
    bj_bitmap*     p_bitmap,
    const bj_rect* p_area,
    uint32_t       pixel
);

////////////////////////////////////////////////////////////////////////////////
/// Draws a filled rectangle in the given bitmap
///
/// The function draws an filled rectangle by filling all pixels within 
/// `p_area`.
///
/// \param p_bitmap The bitmap object.
/// \param p_area   The rectangle to draw.
/// \param pixel    The line pixel value.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_bitmap->width * bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_draw_filled_rectangle(
    bj_bitmap*     p_bitmap,
    const bj_rect* p_area,
    uint32_t       pixel
);

////////////////////////////////////////////////////////////////////////////////
/// Draws the edges of a triangle given its 3 corners.
///
/// \param p_bitmap The bitmap object.
/// \param p0       The first point of the triangle.
/// \param p1       The second point of the triangle.
/// \param p2       The third point of the triangle.
/// \param color    The line color.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_bitmap->width * bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_draw_triangle(
    bj_bitmap* p_bitmap,
    bj_pixel   p0,
    bj_pixel   p1,
    bj_pixel   p2,
    uint32_t   color
);

/// \} // End of drawing group
