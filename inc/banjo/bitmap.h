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

/// Typedef for the \ref bj_bitmap struct
typedef struct bj_bitmap_t bj_bitmap;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap with the specified width and height.
///
/// \param width  Width of the bitmap.
/// \param height Height of the bitmap.
/// \param format The pixel format
/// \return A pointer to the newly created bj_bitmap object.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_new(
    usize           width,
    usize           height,
    bj_pixel_format format
);


////////////////////////////////////////////////////////////////////////////////
/// A set of flags use to customize the conversion of a bitmap to another
///
/// This flagset is used in \ref bj_bitmap_new_from_buffer to provide more
/// information about how the given buffer must be read and converted
///
typedef enum {
    BJ_BITMAP_BUFFER_NONE        = 0x00, ///!< No flag sets.
    BJ_BITMAP_BUFFER_KEEP_STRIDE = 0x01, ///!< The conversion will force the 
                                         ///   bitmap stride to be the same as 
                                         ///   the source buffer.
    BJ_BITMAP_BUFFER_REVERSE_Y   = 0x02, ///!< The bitmap is Y-inverted.
} bj_bitmap_buffer_flags;

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap from the given memory buffer.
///
/// If `buffer` is _0_, this function performs the same as calling
/// `bj_bitmap_new(width, height, format)`.
/// Otherwise, a new bitmap of size `width` * `height` is created and the pixel
/// data is copied from `buffer`.
/// The caller must make sure `format` and `stride` are correct in regard to
/// the given input.
///
/// The generated bitmap may have a different stride than the input buffer's
/// unless `flags` contains `BJ_BITMAP_BUFFER_KEEP_STRIDE`.
///
/// If `BJ_BITMAP_BUFFER_REVERSE_Y` is set in `flags`, the resulting bitmap
/// inverts the resulting bitmap on the vertical (Y) axis.
///
/// \param width  Width of the bitmap.
/// \param height Height of the bitmap.
/// \param format The pixel format
/// \param stride The memory size in bytes of a row of pixel in the input buffer.
/// \param buffer An input buffer of pixel.
/// \param p_error  Pointer to an error object to store any errors encountered during loading.
/// \return A pointer to the newly created bj_bitmap object.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_bitmap_new_from_buffer(
    usize           width,
    usize           height,
    bj_pixel_format format,
    usize           stride,
    void*           buffer,
    int             flags,
    bj_error**      p_error
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bitmap by loading from a file.
///
/// \param p_path   Path to the bitmap file.
/// \param p_error  Pointer to an error object to store any errors encountered during loading.
/// \return A pointer to the newly created bj_bitmap object, or 0 if loading failed.
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
BANJO_EXPORT usize bj_bitmap_width(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the height of the given bitmap
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap height as number of pixels.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT usize bj_bitmap_height(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the pixel format of the given bitmap
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap pixel format.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_bitmap_encoding( 
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Get the number of bytes in a row of pixel data, including the padding.
///
/// \param p_bitmap The bitmap object.
/// \return The bitmap stride
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_bitmap_stride( 
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Change the pixel color at given coordinate.
///
/// \param p_bitmap The bitmap object.
/// \param x        The X coordinate of the pixel.
/// \param y        The Y coordinate of the pixel.
/// \param r        The red component to set
/// \param g        The green component to set
/// \param b        The blue component to set
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_put_rgb(
    bj_bitmap* p_bitmap,
    usize      x,
    usize      y,
    u8         r,
    u8         g,
    u8         b
);

/// \} // End of bitmap group
