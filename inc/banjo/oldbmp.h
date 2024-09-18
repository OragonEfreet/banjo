////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref oldbmp type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup oldbmp Bitmap
/// \ingroup graphics
///
/// \brief The \ref bj_oldbmp object is used to represent 2D data, such as images
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

/// Typedef for the \ref bj_oldbmp struct
typedef struct bj_oldbmp_t bj_oldbmp;

/// \brief Represents a position in a oldbmp.
typedef size_t bj_pixel[2];

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_oldbmp with the specified width and height.
///
/// \param width  Width of the oldbmp.
/// \param height Height of the oldbmp.
/// \return A pointer to the newly created bj_oldbmp object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_oldbmp* bj_oldbmp_new(
    size_t        width,
    size_t        height
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_oldbmp by loading from a file.
///
/// \param p_path   Path to the oldbmp file.
/// \param p_error  Pointer to an error object to store any errors encountered during loading.
/// \return A pointer to the newly created bj_oldbmp object, or NULL if loading failed.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_oldbmp* bj_oldbmp_new_from_file(
    const char*       p_path,
    bj_error**        p_error
);

////////////////////////////////////////////////////////////////////////////////
/// Deletes a bj_oldbmp object and releases associated memory.
///
/// \param p_oldbmp Pointer to the bj_oldbmp object to delete.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_oldbmp_del(
    bj_oldbmp*     p_oldbmp
);

////////////////////////////////////////////////////////////////////////////////
/// Fills the entire oldbmp with the clear color.
///
/// \param p_oldbmp The oldbmp object to reset.
///
/// The clear color can be set with \ref bj_oldbmp_set_clear_color.
/// This function effectively fills all the pixels of the oldbmp with
/// the clear color.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_oldbmp_clear(
    bj_oldbmp*     p_oldbmp
);

////////////////////////////////////////////////////////////////////////////////
/// Sets the color used for clearing the oldbmp.
///
/// \param p_oldbmp    The target oldbmp.
/// \param clear_color The new clear color.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_oldbmp_set_clear_color(
    bj_oldbmp*     p_oldbmp,
    bj_color       clear_color
);

////////////////////////////////////////////////////////////////////////////////
/// Gets the underlying buffer data for direct access.
///
/// \param p_oldbmp The oldbmp object.
/// \return The buffer data.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_color* bj_oldbmp_data(
    bj_oldbmp*     p_oldbmp
);

////////////////////////////////////////////////////////////////////////////////
/// Sets the color of a oldbmp pixel, given its coordinates.
///
/// \param p_oldbmp The oldbmp object.
/// \param x        The X coordinate of the pixel.
/// \param y        The Y coordinate of the pixel.
/// \param color    The color to set at (x, y).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_oldbmp_put(
    bj_oldbmp*     p_oldbmp,
    size_t          x,
    size_t          y,
    bj_color       color
);

////////////////////////////////////////////////////////////////////////////////
/// Gets the color of a oldbmp pixel, given its coordinates.
///
/// \param p_oldbmp The oldbmp object.
/// \param x        The X coordinate of the pixel.
/// \param y        The Y coordinate of the pixel.
/// \return         The color at (x, y).
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_oldbmp->width * bj_oldbmp->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_color bj_oldbmp_get(
    const bj_oldbmp*  p_oldbmp,
    size_t             x,
    size_t             y
);

////////////////////////////////////////////////////////////////////////////////
/// Draws a line of pixels in the given oldbmp.
///
/// The line is drawn for each pixel between p0 and p1.
///
/// \param p_oldbmp The oldbmp object.
/// \param p0       The first point in the line.
/// \param p1       The second point in the line.
/// \param color    The line color.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_oldbmp->width * bj_oldbmp->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_oldbmp_draw_line(
    bj_oldbmp*     p_oldbmp,
    bj_pixel       p0,
    bj_pixel       p1,
    bj_color       color
);

////////////////////////////////////////////////////////////////////////////////
/// Draws the edges of a triangle given its 3 corners.
///
/// \param p_oldbmp The oldbmp object.
/// \param p0       The first point of the triangle.
/// \param p1       The second point of the triangle.
/// \param p2       The third point of the triangle.
/// \param color    The line color.
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up to you to ensure the coordinates lie between
/// [0, bj_oldbmp->width * bj_oldbmp->height].
/// Writing outside of these bounds will result in undefined behavior or 
/// corrupted memory access.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_oldbmp_draw_triangle(
    bj_oldbmp*     p_oldbmp,
    bj_pixel       p0,
    bj_pixel       p1,
    bj_pixel       p2,
    bj_color       color
);

////////////////////////////////////////////////////////////////////////////////
/// Bitmap blitting operation from a source to a destination oldbmp.
///
/// \param p_source           The source bj_oldbmp to copy from.
/// \param p_source_area      The area to copy from in the source oldbmp.
/// \param p_destination      The destination oldbmp.
/// \param p_destination_area The area to copy to in the destination oldbmp.
/// \return                   true if a blit actually happened, false otherwise.
///
/// If p_source_area is NULL, the entire oldbmp is copied.
/// p_destination_area can also be NULL, which is equivalent to
/// using an area at {.x = 0, .y = 0}.
///
/// \par Clipping
///
/// The resulting blit can be clipped if it is performed partially or totally
/// outside of the destination oldbmp.
///
/// p_destination_area.w and p_destination_area.h are ignored for reading
/// but are set to the actual dimensions of the blit.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bool bj_oldbmp_blit(
    const bj_oldbmp*  p_source,
    const bj_rect*    p_source_area,
    bj_oldbmp*        p_destination,
    bj_rect*          p_destination_area
);

/// \} // End of oldbmp group
