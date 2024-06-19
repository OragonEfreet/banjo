////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref bitmap type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup bitmap Bitmap
/// \ingroup graphics
///
/// \brief The \ref bj_bitmap object is used to represent 2D data, such as images
///        and textures.
///
///        The module provides direct drawing primitive function
///
///        Bitmap coordinates are represented with an origin axis set on the 
///        top-left corner.
///        The first axis, usually noted X, extends positively to the right while the
///        second axis, Y, extends to the bottom.
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <banjo/api.h>
#include <banjo/color.h>
#include <banjo/error.h>
#include <banjo/rect.h>

/// Typedef for the \ref bj_bitmap struct
typedef struct bj_bitmap_t bj_bitmap;

/// \brief Represents a position in a bitmap.
typedef usize bj_pixel[2];

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap with the specified width and height.
///
/// \param width  Width of the bitmap.
/// \param height Height of the bitmap.
/// \return A pointer to the newly created bj_bitmap object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_new(
    usize        width,
    usize        height
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap by loading from a file.
///
/// \param p_path   Path to the bitmap file.
/// \param p_error  Pointer to an error object to store any errors encountered during loading.
/// \return A pointer to the newly created bj_bitmap object, or NULL if loading failed.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_new_from_file(
    const char*       p_path,
    bj_error**        p_error
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_bitmap object and releases associated memory.
///
/// \param p_bitmap Pointer to the bj_bitmap object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_del(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Fills the entire bitmap with the clear color.
///
/// \param p_bitmap The bitmap object to reset.
///
/// The clear color can be set with \ref bj_bitmap_set_clear_color.
/// This function effectively fills all the pixels of the bitmap with
/// the clear color.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_clear(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Sets the color used for clearing the bitmap.
///
/// \param p_bitmap    The target bitmap.
/// \param clear_color The new clear color.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_set_clear_color(
    bj_bitmap*     p_bitmap,
    bj_color       clear_color
);

////////////////////////////////////////////////////////////////////////////////
/// Gets the underlying buffer data for direct access.
///
/// \param p_bitmap The bitmap object.
/// \return The buffer data.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_color* bj_bitmap_data(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Sets the color of a bitmap pixel, given its coordinates.
///
/// \param p_bitmap The bitmap object.
/// \param x        The X coordinate of the pixel.
/// \param y        The Y coordinate of the pixel.
/// \param color    The color to set at (x, y).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_put(
    bj_bitmap*     p_bitmap,
    usize          x,
    usize          y,
    bj_color       color
);

////////////////////////////////////////////////////////////////////////////////
/// Gets the color of a bitmap pixel, given its coordinates.
///
/// \param p_bitmap The bitmap object.
/// \param x        The X coordinate of the pixel.
/// \param y        The Y coordinate of the pixel.
/// \return         The color at (x, y).
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_bitmap->width * bj_bitmap->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_color bj_bitmap_get(
    const bj_bitmap*  p_bitmap,
    usize             x,
    usize             y
);

////////////////////////////////////////////////////////////////////////////////
/// Draws a line of pixels in the given bitmap.
///
/// The line is drawn for each pixel between p0 and p1.
///
/// \param p_bitmap The bitmap object.
/// \param p0       The first point in the line.
/// \param p1       The second point in the line.
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
BANJO_EXPORT void bj_bitmap_draw_line(
    bj_bitmap*     p_bitmap,
    bj_pixel       p0,
    bj_pixel       p1,
    bj_color       color
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
    bj_bitmap*     p_bitmap,
    bj_pixel       p0,
    bj_pixel       p1,
    bj_pixel       p2,
    bj_color       color
);

////////////////////////////////////////////////////////////////////////////////
/// Bitmap blitting operation from a source to a destination bitmap.
///
/// \param p_source           The source bj_bitmap to copy from.
/// \param p_source_area      The area to copy from in the source bitmap.
/// \param p_destination      The destination bitmap.
/// \param p_destination_area The area to copy to in the destination bitmap.
/// \return                   true if a blit actually happened, false otherwise.
///
/// If p_source_area is NULL, the entire bitmap is copied.
/// p_destination_area can also be NULL, which is equivalent to
/// using an area at {.x = 0, .y = 0}.
///
/// \par Clipping
///
/// The resulting blit can be clipped if it is performed partially or totally
/// outside of the destination bitmap.
///
/// p_destination_area.w and p_destination_area.h are ignored for reading
/// but are set to the actual dimensions of the blit.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bool bj_bitmap_blit(
    const bj_bitmap*  p_source,
    const bj_rect*    p_source_area,
    bj_bitmap*        p_destination,
    bj_rect*          p_destination_area
);

/// \} // End of bitmap group
