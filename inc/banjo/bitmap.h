////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref bitmap type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup bitmap Bitmap
/// \ingroup graphics
///
/// \brief Matrix of pixels
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <banjo/api.h>
#include <banjo/error.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/stream.h>

/// Represents a pixel position in a bitmap.
typedef int bj_pixel[2];

/// Typedef for the \ref bj_bitmap struct
typedef struct bj_bitmap_t bj_bitmap;

////////////////////////////////////////////////////////////////////////////////
/// Allocate a new bitmap object
///
/// \return A new \ref bj_bitmap instance
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_alloc(
    void
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap with the specified width and height.
///
/// \param width  Width of the bitmap.
/// \param height Height of the bitmap.
/// \param mode   The pixel mode.
/// \param stride The suggested bitmap stride.
///
/// \return A pointer to the newly created bj_bitmap object.
///
/// The stride corresponds to the size in bytes of a row.
/// If the value is less than the required stride, the actual minimum stride
/// is used.
/// Set it to _0_ to automatically compute the stride.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_new(
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
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
/// Creates a new bitmap by loading from a file.
///
/// \param p_path   Path to the bitmap file.
/// \param p_error  Pointer to an error object to store any errors encountered during loading.
/// \return A pointer to the newly created bj_bitmap object, or 0 if loading failed.
///
/// The new object must be deleted using \ref bj_bitmap_del.
///
/// \par Pixel Mode
///
/// Banjo supports the reading or 1, 4, 8, 24 and 32 bits per pixels images.
/// Reading RLE encoding for 4 and 8 bpp is also supported.
///
/// According to [the file compression](https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-compression?redirectedfrom=MSDN]) (MSDN)
/// and the pixel byte size, the created Bitmap will have one of the following
/// pixel mode:
///
/// | Bits Per Pixel | Compression    | `bj_pixel_mode`           |
/// |----------------|----------------|---------------------------|
/// | 1              | `BI_RGB`       | `BJ_PIXEL_MODE_INDEXED_1` |
/// | 4              | `BI_RGB`       | `BJ_PIXEL_MODE_INDEXED_4` |
/// | 4              | `BJ_RLE4`      | `BJ_PIXEL_MODE_INDEXED_4` |
/// | 8              | `BI_RGB`       | `BJ_PIXEL_MODE_INDEXED_8` |
/// | 8              | `BI_RLE8`      | `BJ_PIXEL_MODE_INDEXED_8` |
/// | 16             | `BI_RGB`       | `BJ_PIXEL_MODE_XRGB1555`  |
/// | 16             | `BI_BITFIELDS` | _Depends on bit fields_   |
/// | 24             | `BI_RGB`       | `BJ_PIXEL_MODE_BGR24`     |
/// | 32             | `BI_RGB`       | `BJ_PIXEL_MODE_XRGB8888`  |
/// | 32             | `BI_BITFIELDS` | _Depends on bit fields_   |
///
/// Banjo does not support all bitfield configuration.
/// The following table shows our supported bit fields:
///
/// | Bits Per Pixel | Red Mask     | Green Mask   | Blue Mask    | `bj_pixel_mode`          |
/// |----------------|--------------|--------------|--------------|--------------------------|
/// | 16             | `0x0000F800` | `0x000007E0` | `0x0000001F` | `BJ_PIXEL_MODE_RGB565`   |
/// | 32             | `0x00FF0000` | `0x0000FF00` | `0x000000FF` | `BJ_PIXEL_MODE_XRGB8888` |
///
/// Any other configuration leads to either `BJ_PIXEL_MODE_UNKNOWN` or
/// a failure in loading the file.
///
/// Once loaded, the pixel mode can be retrieved using \ref bj_bitmap_mode.
///
/// \par Edge cases
///
/// - If a bitmap indicates a color palette size but the file does not contain
/// any palette, a same size palette is provided with the first index set to
/// black (red = _0x00_, green = _0x00_, blue = _0x00_) and remaining colors to white
/// (red = _0xFF_, green = _0xFF_, blue = _0xFF_).
/// Banjo does not consider such bitmaps as invalid, but a warning message is
/// logged.
///
///
///
///
/// \par Limitations
///
/// While reading indexed Bitmap works, 1, 4 and 8bpp images are
/// automatically converted to 24bpp images for now.
///
/// \see [BMP file format](https://en.wikipedia.org/wiki/BMP_file_format) (Wikipedia)
/// \see [Bitmap Compression](https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-compression?redirectedfrom=MSDN]) (MSDN)
///
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_new_from_file(
    const char*       p_path,
    bj_error**        p_error
);

BANJO_EXPORT bj_bitmap* bj_bitmap_new_from_pixels(
    void*            p_pixels,
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
);

BANJO_EXPORT bj_bitmap* bj_bitmap_copy(
    const bj_bitmap* p_bitmap
);

BANJO_EXPORT bj_bitmap* bj_bitmap_convert(
    const bj_bitmap* p_bitmap,
    bj_pixel_mode    mode
);

////////////////////////////////////////////////////////////////////////////////
/// Initializes a new bj_bitmap with the specified width and height.
///
/// \param p_bitmap  The bitmap object.
/// \param p_pixels  The pixel buffer data.
/// \param width     Width of the bitmap.
/// \param height    Height of the bitmap.
/// \param mode      The pixel mode.
/// \param stride    The suggested bitmap stride.
/// 
/// If the pixel buffer provided by `p_pixels` is _0_, the buffer will allocate
///
/// \return A pointer to the newly created bj_bitmap object.
///
/// The stride corresponds to the size in bytes of a row.
/// If the value is less than the required stride, the actual minimum stride
/// is used.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_init(
    bj_bitmap* p_bitmap,
    void* p_pixels,
    size_t width,
    size_t height,
    bj_pixel_mode mode,
    size_t stride
);

////////////////////////////////////////////////////////////////////////////////
/// Resets a bj_bitmap object making it ready for a new init or free.
///
/// \param p_bitmap Pointer to the bj_bitmap object to reset.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_reset(
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the underlying pixels data for direct access.
///
/// \param p_bitmap The bitmap object.
/// \return The buffer data.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void* bj_bitmap_pixels(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the width of the given bitmap
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap width as number of pixels.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_bitmap_width(
    const bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the height of the given bitmap
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap height as number of pixels.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_bitmap_height(
    const bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the pixel mode of the given bitmap
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap pixel mode.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_bitmap_mode( 
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the number of bytes in a row of pixel data, including the padding.
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap stride
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_bitmap_stride( 
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Gets the RGB value of a pixel given its 32-bits representation.
///
/// \param p_bitmap   The bitmap object
/// \param x          The X coordinate of the pixel.
/// \param y          The Y coordinate of the pixel.
/// \param p_red      A location to the red component
/// \param p_green    A location to the green component
/// \param p_blue     A location to the blue component
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_rgb(
    const bj_bitmap* p_bitmap,
    size_t           x,
    size_t           y,
    uint8_t*         p_red,
    uint8_t*         p_green,
    uint8_t*         p_blue
);

////////////////////////////////////////////////////////////////////////////////
/// Returns an opaque value representing a pixel color, given its RGB composition.
///
/// \param p_bitmap The bitmap object.
/// \param red      The red component of the color
/// \param green    The green component of the color
/// \param blue     The blue component of the color
/// \return         An opaque `uint32_t` value.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT uint32_t bj_bitmap_pixel_value(
    bj_bitmap* p_bitmap,
    uint8_t    red,
    uint8_t    green,
    uint8_t    blue
);

////////////////////////////////////////////////////////////////////////////////
/// Change the pixel color at given coordinate.
///
/// \param p_bitmap The bitmap object.
/// \param x        The X coordinate of the pixel.
/// \param y        The Y coordinate of the pixel.
/// \param value    The pixel value to put.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_put_pixel(
    bj_bitmap* p_bitmap,
    size_t     x,
    size_t     y,
    uint32_t   value
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
    bj_bitmap* p_bitmap
);

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
BANJO_EXPORT uint32_t bj_bitmap_get(
    const bj_bitmap* p_bitmap,
    size_t           x,
    size_t           y
);

////////////////////////////////////////////////////////////////////////////////
/// Sets the color used for clearing the bitmap.
///
/// \param p_bitmap    The target bitmap.
/// \param clear_color The new clear color.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_set_clear_color(
    bj_bitmap* p_bitmap,
    uint32_t clear_color
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
    const bj_bitmap* p_source,
    const bj_rect* p_source_area,
    bj_bitmap* p_destination,
    const bj_rect* p_destination_area
);

BANJO_EXPORT bool bj_bitmap_blit_stretched(
    const bj_bitmap* p_source,
    const bj_rect* p_source_area,
    bj_bitmap* p_destination,
    const bj_rect* p_destination_area
);




/// \} // End of bitmap group
