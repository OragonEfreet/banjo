////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref bitmap type.
#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \defgroup bitmap Framebuffer
/// \ingroup graphics
///
/// The \ref bj_bitmap object is used to represent 2D data, such as images
/// and textures.
///
/// The module provides direct drawing primitive function
///
/// Framebuffer coordinates are represented with an origin axis set on the 
/// top-left corner.
/// The first axis, usually noted X, extends positively to thr right while the
/// second axis, Y, extends to the bottom.
#include <banjo/api.h>
#include <banjo/color.h>
#include <banjo/error.h>
#include <banjo/rect.h>

/// Typedef for the \ref bj_bitmap struct
typedef struct bj_bitmap_t bj_bitmap;

/// \brief Represents a position in a bitmap.
typedef usize bj_pixel[2];

/// \brief The internal data structure for the \ref bj_bitmap type.
struct bj_bitmap_t {
    usize     width;       ///< Width of the bitmap (X axis).
    usize     height;      ///< Height of the bitmap (Y axis).
    bj_color  clear_color; ///< Framebuffer data
    bj_color* buffer;      ///< Framebuffer data
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a new bitmap given its size
///
/// \param p_bitmap The bitmap object to initialize
/// \param width         The bitmap width in pixels
/// \param height        The bitmap height in pixels
///
/// \return `p_bitmap`
///
/// The created buffer data isn't initialized.
/// To fill the data with an initial value, call \ref bj_bitmap_clear.
///
/// The clear color for a default initialized bitmap is \ref BJ_COLOR_BLACK.
/// This can be changed with \ref bj_bitmap_set_clear_color.
BANJO_EXPORT bj_bitmap* bj_bitmap_init_default(
    bj_bitmap*   p_bitmap,
    usize        width,
    usize        height
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a new bitmap by loading a BMP file
///
/// \param p_bitmap      The bitmap object to initialize
/// \param p_path        Path to the input file
/// \param p_error       Error object
///
/// \return `p_bitmap`
///
BANJO_EXPORT bj_bitmap* bj_bitmap_init_from_file(
    bj_bitmap*        p_bitmap,
    const char*       p_path,
    bj_error**        p_error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resets the bitmap to Nil state
///
/// \param p_bitmap The bitmap object to reset.
///
/// \return `p_bitmap`
///
/// Once reset, `p_bitmap` represents a 0x0 sized bitmap
/// 
BANJO_EXPORT bj_bitmap* bj_bitmap_reset(
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Fills the entire bitmap with the clear color.
///
/// \param p_bitmap The bitmap object to reset.
///
/// The clear color can be set with \ref bj_bitmap_set_clear_color.
/// This function effectivement fills all the pixels of the bitmap with
/// the clear color.
///
BANJO_EXPORT void bj_bitmap_clear(
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Sets the color used for clearing the bitmap
///
/// The bitmap can be cleared using \ref bj_bitmap_clear.
///
/// \param p_bitmap The target bitmap
/// \param clear_color The new clear color
///
BANJO_EXPORT void bj_bitmap_set_clear_color(
    bj_bitmap* p_bitmap,
    bj_color clear_color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the underlying buffer data for direct access
///
/// \param p_bitmap The bitmap object
///
/// \return The buffer data
///
BANJO_EXPORT bj_color* bj_bitmap_data(
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the color of a bitmap pixel, given its coordinates
///
/// \param p_bitmap The bitmap object
/// \param x The X coordinate of the pixel
/// \param y The Y coordinate of the pixel
/// \param color The color to set at _x, y_.
///
BANJO_EXPORT void bj_bitmap_put(
    bj_bitmap* p_bitmap,
    usize x,
    usize y,
    bj_color color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the color of a bitmap pixel, given its coordinates
///
/// \param p_bitmap The bitmap object
/// \param x The X coordinate of the pixel
/// \param y The Y coordinate of the pixel
///
/// \return a \ref bj_color object
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up you to ensure the coordinates lie between
/// _[0 ; bj_bitmap->width * bj_bitmap->height]_.
/// Writing outside of these bounds _will_ result in an undefined behavior or 
/// a corrupted memory access.
///
BANJO_EXPORT bj_color bj_bitmap_get(
    const bj_bitmap* p_bitmap,
    usize            x,
    usize            y
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw a line of pixels in the given bitmap
///
/// The line is drawn for each pixel between `p0` and `p1`.
///
/// \param p_bitmap The bitmap object
/// \param p0            The first point in the line.
/// \param p1            The second point in the line.
/// \param color         The line color
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up you to ensure the coordinates lie between
/// _[0 ; bj_bitmap->width * bj_bitmap->height]_.
/// Writing outside of these bounds _will_ result in an undefined behavior or 
/// a corrupted memory access.
///
BANJO_EXPORT void bj_bitmap_draw_line(
    bj_bitmap* p_bitmap,
    bj_pixel p0,
    bj_pixel p1,
    bj_color color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw the edges of a triangle given its 3 corners.
///
/// \param p_bitmap The bitmap object
/// \param p0            The first point of the triangle.
/// \param p1            The second point of the triangle.
/// \param p2            The third point of the triangle.
/// \param color         The line color
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up you to ensure the coordinates lie between
/// _[0 ; bj_bitmap->width * bj_bitmap->height]_.
/// Writing outside of these bounds _will_ result in an undefined behavior or 
/// a corrupted memory access.
///
BANJO_EXPORT void bj_bitmap_draw_triangle(
    bj_bitmap* p_bitmap,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    bj_color color
);


////////////////////////////////////////////////////////////////////////////////
/// Bitmap blitting operation from a source to a destination bitmap
///
/// \param p_source The \ref bj_bitmap to copy from.
/// \param p_source_area The area to copy from in the source bitmap.
/// \param p_destination The destination bitmap.
/// \param p_destination_area The area to copy to in the destination bitmap.
///
/// \return _true_ if a blit actuall happened, _false_ otherwise.
///
/// If `p_source_area` is _0_, the entire bitmap is copied.
/// `p_destination_area` can also be set to _0_, which is equivalent to
/// using an area at _.x = 0, .y = 0_.
///
/// \par Clipping
///
/// The resulting blit can be clipped it is performed partially or totally
/// outside of the destination bitmap.
///
/// `p_destination_area.w` and `p_destination_area.h` are ignored for reading
/// but are set to the actual dimensions of the blit.
///
BANJO_EXPORT bool bj_bitmap_blit(
    const bj_bitmap* p_source,
    const bj_rect*   p_source_area,
    bj_bitmap*       p_destination,
    bj_rect*         p_destination_area
);

