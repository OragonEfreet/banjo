////////////////////////////////////////////////////////////////////////////////
/// \file pixel.h
/// Header file for general pixel manipulation facilities.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup pixel Pixel Definition
/// \ingroup bitmap
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_PIXEL_H
#define BJ_PIXEL_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Representation of a pixel encoding.
enum bj_pixel_mode {
    BJ_PIXEL_MODE_UNKNOWN   = 0x00u,       //!< Unknown/Invalid pixel mode
    BJ_PIXEL_MODE_INDEXED_1 = 0x00000101u, //!< 1bpp indexed
    BJ_PIXEL_MODE_INDEXED_4 = 0x00000104u, //!< 4bpp indexed
    BJ_PIXEL_MODE_INDEXED_8 = 0x00000108u, //!< 8bpp indexed

    BJ_PIXEL_MODE_XRGB1555  = 0x01000210u, //!< 16bpp 555-RGB
    BJ_PIXEL_MODE_RGB565    = 0x01020210u, //!< 16bpp 565-RGB
    BJ_PIXEL_MODE_XRGB8888  = 0x01010220u, //!< 32bpp RGB

    BJ_PIXEL_MODE_BGR24     = 0x02000318u, //!< 24bpp BGR
};
#ifndef BJ_NO_TYPEDEF
typedef enum bj_pixel_mode bj_pixel_mode;
#endif

////////////////////////////////////////////////////////////////////////////////
/// Gets the RGB value of a pixel given its 32-bits representation.
///
/// \param mode  The pixel mode
/// \param value The opaque pixel value
/// \param red   A location to the red component
/// \param green A location to the green component
/// \param blue  A location to the blue component
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_make_pixel_rgb(
    enum bj_pixel_mode mode,
    uint32_t      value,
    uint8_t*      red,
    uint8_t*      green,
    uint8_t*      blue
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
BANJO_EXPORT uint32_t bj_get_pixel_value(
    enum bj_pixel_mode mode, 
    uint8_t red,
    uint8_t green,
    uint8_t blue
);

////////////////////////////////////////////////////////////////////////////////
/// Determine the most suitable bj_pixel_mode from a set of masks.
///
/// This function can be used to retrieve what Banjo consider as the pixel
/// mode corresponding to a given depth and masks.
///
/// If not suitable pixel mode is found, BJ_PIXEL_MODE_UNKNOWN is returned.
///
/// \param bpp        The number of bits used to encode a pixel.
/// \param red_mask   A bitmask for the red channel information in the bits.
/// \param green_mask A bitmask for the green channel information in the bits.
/// \param blue_mask  A bitmask for the blue channel information in the bits.
/// \return A  bj_pixel_mode value.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_compute_pixel_mode(
    uint8_t  bpp,
    uint32_t red_mask,
    uint32_t green_mask,
    uint32_t blue_mask
);

////////////////////////////////////////////////////////////////////////////////
/// Returns the stride used for encoding a bitmaps in Banjo
///
/// In a bitmap, the stride is the actual number of bytes used to encode a 
/// single row of pixels.
/// Hence, the total size needed for a bitmap of `width` x `height` encoded with
/// `mode` is equal to `bj_compute_bitmap_stride(width, mode) * height`.
///
/// Banjo uses the bits-per-pixel information from `mode` to compute the stride
/// and aligns the byte count to 4.
///
/// \param width The width in pixel of a row
/// \param mode  The pixel mode
/// \return      The bitmap stride in bytes
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_compute_bitmap_stride(
    size_t width,
    enum bj_pixel_mode mode
);

/// Pixel type: Indexed (palette-based).
#define BJ_PIXEL_TYPE_INDEX    0x01
/// Pixel type: Bitfield representation (e.g., RGB565).
#define BJ_PIXEL_TYPE_BITFIELD 0x02
/// Pixel type: Byte-packed representation (e.g., RGBA8888).
#define BJ_PIXEL_TYPE_BYTES    0x03

/// Pixel order: Red-Green-Blue (RGB).
#define BJ_PIXEL_ORDER_RGB  0x01
/// Pixel order: Same as BJ_PIXEL_ORDER_RGB with unused alpha.
#define BJ_PIXEL_ORDER_XRGB BJ_PIXEL_ORDER_RGB
/// Pixel order: Blue-Green-Red (BGR).
#define BJ_PIXEL_ORDER_BGR  0x02
/// Pixel order: Same as BJ_PIXEL_ORDER_BGR with unused alpha.
#define BJ_PIXEL_ORDER_XBGR BJ_PIXEL_ORDER_BGR
/// Pixel order: Red-Green-Blue with padding byte (RGBX).
#define BJ_PIXEL_ORDER_RGBX 0x03
/// Pixel order: Blue-Green-Red with padding byte (BGRX).
#define BJ_PIXEL_ORDER_BGRX 0x04
/// Pixel order: Alpha-Red-Green-Blue (ARGB).
#define BJ_PIXEL_ORDER_ARGB 0x05
/// Pixel order: Alpha-Blue-Green-Red (ABGR).
#define BJ_PIXEL_ORDER_ABGR 0x06
/// Pixel order: Red-Green-Blue-Alpha (RGBA).
#define BJ_PIXEL_ORDER_RGBA 0x07
/// Pixel order: Blue-Green-Red-Alpha (BGRA).
#define BJ_PIXEL_ORDER_BGRA 0x08

/// Pixel layout: 16-bit with 1-bit alpha, 5-bit red, green, and blue (1555).
#define BJ_PIXEL_LAYOUT_1555 0x00
/// Pixel layout: 32-bit with 8 bits per channel (8888).
#define BJ_PIXEL_LAYOUT_8888 0x01
/// Pixel layout: 16-bit with 5 bits for red and blue, 6 bits for green (565).
#define BJ_PIXEL_LAYOUT_565  0x02

////////////////////////////////////////////////////////////////////////////////
/// Creates a pixel format mode from bits-per-pixel, type, layout, and order.
///
/// \param bpp Bits-per-pixel.
/// \param type Pixel type (e.g., indexed, bitfield, or bytes).
/// \param layout Pixel layout (e.g., 1555, 8888, or 565).
/// \param order Pixel order (e.g., RGB, ARGB, etc.).
#define BJ_PIXEL_MODE_MAKE(bpp, type, layout, order) (((order & 0xFF) << 24) | ((layout & 0xFF) << 16) | ((type & 0xFF) << 8) | (bpp & 0xFF))

////////////////////////////////////////////////////////////////////////////////
/// Creates a pixel format for indexed (palette-based) pixels.
///
/// \param bpp Bits-per-pixel.
#define BJ_PIXEL_MODE_MAKE_INDEXED(bpp)               BJ_PIXEL_MODE_MAKE(bpp, BJ_PIXEL_TYPE_INDEX, 0, 0)

////////////////////////////////////////////////////////////////////////////////
/// Creates a 16-bit bitfield pixel format.
///
/// \param layout Pixel layout (e.g., 1555, 565).
/// \param order Pixel order (e.g., RGB, ARGB).
#define BJ_PIXEL_MODE_MAKE_BITFIELD_16(layout, order) BJ_PIXEL_MODE_MAKE(16, BJ_PIXEL_TYPE_BITFIELD, layout, order)

////////////////////////////////////////////////////////////////////////////////
/// Creates a 32-bit bitfield pixel format.
///
/// \param layout Pixel layout (e.g., 8888).
/// \param order Pixel order (e.g., RGB, ARGB).
#define BJ_PIXEL_MODE_MAKE_BITFIELD_32(layout, order) BJ_PIXEL_MODE_MAKE(32, BJ_PIXEL_TYPE_BITFIELD, layout, order)

////////////////////////////////////////////////////////////////////////////////
/// Creates a byte-packed pixel format.
///
/// \param bpp Bits-per-pixel.
/// \param order Pixel order (e.g., RGB, ARGB).
#define BJ_PIXEL_MODE_MAKE_BYTES(bpp, order)          BJ_PIXEL_MODE_MAKE(bpp, BJ_PIXEL_TYPE_BYTES, 0, order)

////////////////////////////////////////////////////////////////////////////////
/// Extracts the bits-per-pixel from a pixel format.
///
/// \param fmt Pixel format value.
#define BJ_PIXEL_GET_BPP(fmt)    ((fmt) & 0xFF)

////////////////////////////////////////////////////////////////////////////////
/// Extracts the pixel type from a pixel format.
///
/// \param fmt Pixel format value.
#define BJ_PIXEL_GET_TYPE(fmt)   (((fmt) >> 8) & 0xFF)

////////////////////////////////////////////////////////////////////////////////
/// Extracts the pixel layout from a pixel format.
///
/// \param fmt Pixel format value.
#define BJ_PIXEL_GET_LAYOUT(fmt) (((fmt) >> 16) & 0xFF)

////////////////////////////////////////////////////////////////////////////////
/// Extracts the pixel order from a pixel format.
///
/// \param fmt Pixel format value.
#define BJ_PIXEL_GET_ORDER(fmt)  (((fmt) >> 24) & 0xFF)

#endif
/// \} 
