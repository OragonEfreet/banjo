////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for general pixel manipulation facilities.
////////////////////////////////////////////////////////////////////////////////
/// \ingroup graphics
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>


////////////////////////////////////////////////////////////////////////////////
/// \brief Representation of a pixel encoding.
typedef enum {
    BJ_PIXEL_MODE_UNKNOWN   = 0x00u,
    BJ_PIXEL_MODE_INDEXED_1 = 0x00000101u,
    BJ_PIXEL_MODE_INDEXED_4 = 0x00000104u,
    BJ_PIXEL_MODE_INDEXED_8 = 0x00000108u,

    BJ_PIXEL_MODE_XRGB1555  = 0x01000210u,
    BJ_PIXEL_MODE_RGB565    = 0x01020210u,
    BJ_PIXEL_MODE_XRGB8888  = 0x01010220u,

    BJ_PIXEL_MODE_BGR24     = 0x02000318u,
} bj_pixel_mode;

////////////////////////////////////////////////////////////////////////////////
/// Gets the RGB value of a pixel given its 32-bits representation.
///
/// \param mode       The pixel mode
/// \param value      The opaque pixel value
/// \param p_red      A location to the red component
/// \param p_green    A location to the green component
/// \param p_blue     A location to the blue component
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_pixel_rgb(
    bj_pixel_mode mode,
    uint32_t      value,
    uint8_t*      p_red,
    uint8_t*      p_green,
    uint8_t*      p_blue
);

////////////////////////////////////////////////////////////////////////////////
/// Returns an opaque value representing a pixel color, given its RGB composition.
///
/// \param mode     The pixel mode
/// \param red      The red component of the color
/// \param green    The green component of the color
/// \param blue     The blue component of the color
/// \return         An opaque `uint32_t` value.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint32_t bj_pixel_value(
    bj_pixel_mode mode, 
    uint8_t red,
    uint8_t green,
    uint8_t blue
);


/// \} 
