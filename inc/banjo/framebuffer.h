////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref framebuffer type.
#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \defgroup framebuffer Framebuffer
/// \ingroup graphics
///
/// The \ref bj_framebuffer object is used to represent 2D data, such as images
/// and textures.
///
/// The module provides direct drawing primitive function
///
/// Framebuffer coordinates are represented with an origin axis set on the 
/// top-left corner.
/// The first axis, usually noted X, extends positively to thr right while the
/// second axis, Y, extends to the bottom.
#include <banjo/api.h>
#include <banjo/array.h>
#include <banjo/color.h>

/// Typedef for the \ref bj_framebuffer struct
typedef struct bj_framebuffer_t bj_framebuffer;

/// \brief Represents a position in a framebuffer.
typedef usize bj_pixel[2];

/// \brief The internal data structure for the \ref bj_framebuffer type.
struct bj_framebuffer_t {
    usize width;           ///< Width of the framebuffer (X axis).
    usize height;          ///< Height of the framebuffer (Y axis).
    bj_color  clear_color; ///< Framebuffer data
    bj_color* buffer;      ///< Framebuffer data
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes a new framebuffer given its size
///
/// \param p_framebuffer The framebuffer object to initialize
/// \param width         The framebuffer width in pixels
/// \param height        The framebuffer height in pixels
///
/// \return `p_framebuffer`
///
/// The created buffer data isn't initialized.
/// To fill the data with an initial value, call \ref bj_framebuffer_clear.
///
/// The clear color for a default initialized framebuffer is \ref BJ_COLOR_BLACK.
/// This can be changed with \ref bj_framebuffer_set_clear_color.
BANJO_EXPORT bj_framebuffer* bj_framebuffer_init_default(
    bj_framebuffer*   p_framebuffer,
    usize             width,
    usize             height
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resets the framebuffer to Nil state
///
/// \param p_framebuffer The framebuffer object to reset.
///
/// \return `p_framebuffer`
///
/// Once reset, `p_framebuffer` represents a 0x0 sized framebuffer
/// 
BANJO_EXPORT bj_framebuffer* bj_framebuffer_reset(
    bj_framebuffer* p_framebuffer
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Fills the entire framebuffer with the clear color.
///
/// \param p_framebuffer The framebuffer object to reset.
///
/// The clear color can be set with \ref bj_framebuffer_set_clear_color.
/// This function effectivement fills all the pixels of the framebuffer with
/// the clear color.
///
BANJO_EXPORT void bj_framebuffer_clear(
    bj_framebuffer* p_framebuffer
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Sets the color used for clearing the framebuffer
///
/// The framebuffer can be cleared using \ref bj_framebuffer_clear.
///
/// \param p_framebuffer The target framebuffer
/// \param clear_color The new clear color
///
BANJO_EXPORT void bj_framebuffer_set_clear_color(
    bj_framebuffer* p_framebuffer,
    bj_color clear_color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Gets the underlying buffer data for direct access
///
/// \param p_framebuffer The framebuffer object
///
/// \return The buffer data
///
BANJO_EXPORT bj_color* bj_framebuffer_data(
    bj_framebuffer* p_framebuffer
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Set the color of a framebuffer pixel, given its coordinates
///
/// \param p_framebuffer The framebuffer object
/// \param x The X coordinate of the pixel
/// \param y The Y coordinate of the pixel
/// \param color The color to set at _x, y_.
///
BANJO_EXPORT void bj_framebuffer_put(
    bj_framebuffer* p_framebuffer,
    usize x,
    usize y,
    bj_color color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the color of a framebuffer pixel, given its coordinates
///
/// \param p_framebuffer The framebuffer object
/// \param x The X coordinate of the pixel
/// \param y The Y coordinate of the pixel
///
/// \return a \ref bj_color object
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up you to ensure the coordinates lie between
/// _[0 ; bj_framebuffer->width * bj_framebuffer->height]_.
/// Writing outside of these bounds _will_ result in an undefined behavior or 
/// a corrupted memory access.
///
BANJO_EXPORT bj_color bj_framebuffer_get(
    bj_framebuffer* p_framebuffer,
    usize x,
    usize y
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw a line of pixels in the given framebuffer
///
/// The line is drawn for each pixel between `p0` and `p1`.
///
/// \param p_framebuffer The framebuffer object
/// \param p0            The first point in the line.
/// \param p1            The second point in the line.
/// \param color         The line color
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up you to ensure the coordinates lie between
/// _[0 ; bj_framebuffer->width * bj_framebuffer->height]_.
/// Writing outside of these bounds _will_ result in an undefined behavior or 
/// a corrupted memory access.
///
BANJO_EXPORT void bj_framebuffer_draw_line(
    bj_framebuffer* p_framebuffer,
    bj_pixel p0,
    bj_pixel p1,
    bj_color color
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Draw the edges of a triangle given its 3 corners.
///
/// \param p_framebuffer The framebuffer object
/// \param p0            The first point of the triangle.
/// \param p1            The second point of the triangle.
/// \param p2            The third point of the triangle.
/// \param color         The line color
///
/// \par Memory Safety
///
/// This function does not perform any bound checking on the pixel coordinates.
/// It is up you to ensure the coordinates lie between
/// _[0 ; bj_framebuffer->width * bj_framebuffer->height]_.
/// Writing outside of these bounds _will_ result in an undefined behavior or 
/// a corrupted memory access.
///
BANJO_EXPORT void bj_framebuffer_draw_triangle(
    bj_framebuffer* p_framebuffer,
    bj_pixel p0,
    bj_pixel p1,
    bj_pixel p2,
    bj_color color
);

