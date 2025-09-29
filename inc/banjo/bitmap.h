////////////////////////////////////////////////////////////////////////////////
/// \file bitmap.h
/// Header file for \ref bitmap type.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup bitmap Bitmap
///
/// \brief Matrix of pixels
///
/// 2D pixel buffers and blit utilities. Create, destroy, load from files,
/// convert pixel modes, access pixels, and render via blit, masked blit,
/// stretched blit, and text drawing. Integrates with \ref bj_pixel_mode,
/// \ref bj_rect, and window framebuffers.
///
/// Typical usage:
/// - Create with \ref bj_create_bitmap or \ref bj_create_bitmap_from_file.
/// - Query size and mode, or access raw pixels.
/// - Draw: \ref bj_blit, \ref bj_blit_stretched, \ref bj_blit_mask,
///   \ref bj_blit_mask_stretched, \ref bj_draw_text, \ref bj_blit_text.
/// - Destroy with \ref bj_destroy_bitmap.
///
/// \note Public APIs use destination-native packed colors unless stated
///       otherwise. Use \ref bj_make_bitmap_pixel to pack values.
///
/// \todo Add support for writing bitmaps to disk (e.g., BMP/PNG) with a
///       `bj_write_bitmap_*` API.
///
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_BITMAP_H
#define BJ_BITMAP_H
#include <banjo/api.h>
#include <banjo/error.h>
#include <banjo/pixel.h>
#include <banjo/rect.h>
#include <banjo/stream.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Raster operation (ROP) to apply during blitting.
///
/// These operations define how source pixels combine with destination pixels
/// during \ref bj_blit and \ref bj_blit_stretched.
/// Some operations are optimized on specific formats (e.g., 32bpp).
///
/// \note For mismatched pixel formats, colors are combined in linear integer
/// RGB (8-bit per channel) after conversion from/to native formats.
///
typedef enum {
    BJ_BLIT_OP_COPY = 0,  //!< Copy source to destination (fast path when formats match)
    BJ_BLIT_OP_XOR,       //!< Bitwise XOR (channel-wise for >8bpp)
    BJ_BLIT_OP_OR,        //!< Bitwise OR
    BJ_BLIT_OP_AND,       //!< Bitwise AND
    BJ_BLIT_OP_ADD_SAT,   //!< Per-channel saturated add (clamped to 255)
    BJ_BLIT_OP_SUB_SAT,   //!< Per-channel saturated subtract (clamped to 0)
} bj_blit_op;



/// Typedef for the \ref bj_bitmap struct
typedef struct bj_bitmap_t bj_bitmap;

////////////////////////////////////////////////////////////////////////////////
/// Allocate a new bitmap object
///
/// \return A new \ref bj_bitmap instance
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_allocate_bitmap(
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
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_create_bitmap(
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
BANJO_EXPORT void bj_destroy_bitmap(
    bj_bitmap*     p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bitmap by loading from a file.
///
/// \param p_path   Path to the bitmap file.
/// \param p_error  Pointer to an error object to store any errors encountered during loading.
/// \return A pointer to the newly created bj_bitmap object, or 0 if loading failed.
///
/// The new object must be deleted using \ref bj_destroy_bitmap.
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
BANJO_EXPORT bj_bitmap* bj_create_bitmap_from_file(
    const char*       p_path,
    bj_error**        p_error
);


////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap with the specified width and height.
///
/// Contrary to \ref bj_create_bitmap, the pixel data is explicitely provided
/// by the caller through `p_pixels`.
/// The caller is reponsible for ensuring the allocated pixel data matches
/// `width`, `height`, `mode` and `stride`.
///
/// You can use \ref bj_compute_bitmap_stride with `width` and `mode` to
/// retrieve the most suitable value for `stride`.
/// It's also possible to set `stride` to _0_ and let Banjo compute it
/// automatically.
///
/// \param p_pixels A pre-allocated array of pixels
/// \param width  Width of the bitmap.
/// \param height Height of the bitmap.
/// \param mode   The pixel mode.
/// \param stride The suggested bitmap stride.
/// \return A new instance of \ref bj_bitmap.
///
/// \par Behaviour
///
/// Returns _0_ if `p_pixels` is _0_.
///
/// \par Memory Management
///
/// The caller is responsible for the memory management of `p_pixels`.
/// \ref bj_bitmap will not modify it and it will not be released upon calling
/// \ref bj_destroy_bitmap.
///
/// The caller is responsible for releasing the bitmap using \ref bj_destroy_bitmap.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_create_bitmap_from_pixels(
    void*            p_pixels,
    size_t           width,
    size_t           height,
    bj_pixel_mode    mode,
    size_t           stride
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap by copying `p_bitmap`.
///
/// \param p_bitmap The source bitmap.
///
/// \return A pointer to the newly created bj_bitmap object.
///
/// The new bitmap will have exactly the same properties than the source bitmap.
///
/// \par Memory Management
///
/// The caller is responsible from releasing the bitmap using 
/// \ref bj_destroy_bitmap.
///
/// If `p_source` was initially created using \ref bj_create_bitmap_from_pixels,
/// the "weak" property of the bitmap is not maintained in the new bitmap and
/// the caller is not responsible for manually releasing its pixel data.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_copy_bitmap(
    const bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Creates a new bj_bitmap by converting `p_bitmap`.
///
/// \param p_bitmap The source bitmap.
/// \param mode     The new pixel mode.
///
/// \return A pointer to the newly created bj_bitmap object.
///
/// The new bitmap is provided by creating a new empty bitmap matching source's
/// width and height, but using `mode` as pixel mode.
/// The pixels are then converted to fill-in the new buffer.
///
/// \par Behaviour
///
/// Returns `bj_copy_bitmap(p_bitmap)` if `mode == bj_bitmap_mode(p_bitmap)`.
///
/// Returns _0_ if `mode` is \ref BJ_PIXEL_MODE_UNKNOWN or an unsupported value.
///
/// \par Memory Management
///
/// The caller is responsible from releasing the bitmap using 
/// \ref bj_destroy_bitmap.
///
/// If `p_bitmap` was initially created using \ref bj_create_bitmap_from_pixels,
/// the "weak" property of the bitmap is not maintained in the new bitmap and
/// the caller is not responsible for manually releasing its pixel data.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bitmap* bj_convert_bitmap(
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
BANJO_EXPORT bj_bitmap* bj_init_bitmap(
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
BANJO_EXPORT void bj_reset_bitmap(
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
BANJO_EXPORT void bj_make_bitmap_rgb(
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
BANJO_EXPORT uint32_t bj_make_bitmap_pixel(
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
BANJO_EXPORT void bj_put_pixel(
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
/// The clear color can be set with \ref bj_set_bitmap_clear_color.
/// This function effectively fills all the pixels of the bitmap with
/// the clear color.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_clear_bitmap(
    bj_bitmap* p_bitmap
);

////////////////////////////////////////////////////////////////////////////////
/// Enables or disables color key transparency for blitting.
///
/// \param p_bitmap   The target bitmap.
/// \param enabled    Whether the color key should be enabled.
/// \param key_value  The pixel value (in bitmap's native format) considered transparent.
///
/// When color keying is enabled on the **source** bitmap, blitters skip any
/// source pixel equal to `key_value`.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_set_bitmap_colorkey(
    bj_bitmap*  p_bitmap,
    bj_bool     enabled,
    uint32_t    key_value
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
BANJO_EXPORT uint32_t bj_bitmap_pixel(
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
BANJO_EXPORT void bj_set_bitmap_clear_color(
    bj_bitmap* p_bitmap,
    uint32_t clear_color
);

////////////////////////////////////////////////////////////////////////////////
/// Bitmap blitting operation from a source to a destination bitmap.
///
/// \param src        The source bitmap.
/// \param src_area   Optional area to copy from in the source bitmap (0 = full source).
/// \param dst        The destination bitmap.
/// \param dst_area   Optional area to copy to in the destination bitmap (0 = same size at {0,0}).
/// \param op         The raster operation to apply (see \ref bj_blit_op).
/// \return           *BJ_TRUE* if a blit actually happened, *BJ_FALSE* otherwise.
///
/// If `src_area` is 0, the entire source is copied.
/// If `dst_area` is 0, the destination area defaults to `{.x=0,.y=0,.w=src_area.w,.h=src_area.h}`.
///
/// \par Clipping
///
/// The blit is automatically clipped to the destination bounds. If clipping
/// occurs, the source area is adjusted accordingly to preserve pixel mapping.
///
/// \par Color Key
///
/// If the *source* bitmap has color keying enabled via \ref bj_set_bitmap_colorkey,
/// any source pixel equal to the key value is skipped.
///
/// \par Pixel Formats
///
/// - If `src->mode == dst->mode` and `op == BJ_BLIT_OP_COPY`, a fast path is used.
/// - Otherwise, pixels are converted via RGB and the ROP is applied per channel.
///
/// \par Limitations
///
/// Sub-byte formats (1/4/8bpp) are supported but may be slower due to bit packing.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_blit(
    const bj_bitmap* src, const bj_rect* src_area,
    bj_bitmap* dst, const bj_rect* dst_area,
    bj_blit_op op);

////////////////////////////////////////////////////////////////////////////////
/// Stretched bitmap blitting (nearest neighbor).
///
/// \param src        The source bitmap.
/// \param src_area   Optional area to copy from in the source bitmap (0 = full source).
/// \param dst        The destination bitmap.
/// \param dst_area   Optional area to copy to in the destination bitmap (0 = full destination).
/// \param op         The raster operation to apply (see \ref bj_blit_op).
/// \return           *BJ_TRUE* if a blit actually happened, *BJ_FALSE* otherwise.
///
/// If source and destination rectangles have the same size, this function
/// delegates to \ref bj_blit and uses the same fast paths.
///
/// \par Clipping
///
/// The destination rectangle is clipped to the destination bounds, and the
/// source rectangle is proportionally adjusted to ensure visual consistency.
///
/// \par Color Key
///
/// Color keying on the *source* bitmap is honored (see \ref bj_set_bitmap_colorkey).
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_blit_stretched(
    const bj_bitmap* src, const bj_rect* src_area,
    bj_bitmap* dst, const bj_rect* dst_area,
    bj_blit_op op);

////////////////////////////////////////////////////////////////////////////////
/// Mask background mode for masked blits (glyph/text rendering).
///
/// \par Modes
/// - `BJ_MASK_BG_TRANSPARENT`:
///     Only the foreground is drawn where the mask coverage > 0. Destination
///     pixels outside the mask are preserved (no background fill).
/// - `BJ_MASK_BG_OPAQUE`:
///     The entire destination rectangle is written as a blend between the
///     background color (where mask coverage is 0) and the foreground color
///     (where mask coverage is 255), with linear interpolation for values in
///     between.
/// - `BJ_MASK_BG_REV_TRANSPARENT`:
///     Carved mode. The background color is composited where the mask coverage
///     is **outside** the glyph (i.e., with alpha = 1 - coverage). Pixels inside
///     the glyph (coverage=255) keep the destination value, effectively cutting
///     the text out of the background.
///
typedef enum {
    BJ_MASK_BG_TRANSPARENT = 0,   //!< Foreground over destination where mask>0
    BJ_MASK_BG_OPAQUE,            //!< Opaque band: mix(background, foreground, mask)
    BJ_MASK_BG_REV_TRANSPARENT    //!< Carved: mix(destination, background, 1-mask)
} bj_mask_bg_mode;

////////////////////////////////////////////////////////////////////////////////
/// Masked blit (non-stretched). The mask must be 8bpp (coverage 0..255).
///
/// \param mask        The 8bpp mask bitmap (0 = fully transparent, 255 = fully opaque).
/// \param mask_area   Optional area in the mask to use (0 = full mask).
/// \param dst         The destination bitmap.
/// \param dst_area    Optional destination area (0 = place at {0,0} with mask_area size).
/// \param fg_native   Foreground color packed in the destination's native format.
/// \param bg_native   Background color packed in the destination's native format.
/// \param mode        The background mode (see \ref bj_mask_bg_mode).
/// \return            *BJ_TRUE* if any pixel was written, *BJ_FALSE* otherwise.
///
/// \par Behavior
///
/// - For `BJ_MASK_BG_TRANSPARENT`, foreground is **source-over** composited
///   onto destination wherever the mask is non-zero.
/// - For `BJ_MASK_BG_OPAQUE`, each pixel in the dest area is `mix(bg, fg, mask)`.
/// - For `BJ_MASK_BG_REV_TRANSPARENT`, the background is composited with
///   alpha `(1 - mask)` and glyph interiors are left untouched.
///
/// \par Pixel Formats
///
/// The mask must be 8 bits per pixel. Destination can be any supported pixel
/// mode; colors must be provided in destination-native format (see
/// \ref bj_make_bitmap_pixel).
///
/// \par Clipping
///
/// The destination area is clipped to the destination bounds. The mask area
/// is clipped to the mask bounds. Both rectangles must have identical sizes.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_blit_mask(
    const bj_bitmap* mask,
    const bj_rect*   mask_area,     /* NULL = full mask */
    bj_bitmap*       dst,
    const bj_rect*   dst_area,      /* NULL = place at {0,0} w/ mask_area size */
    uint32_t         fg_native,     /* dst-native packed color */
    uint32_t         bg_native,     /* dst-native packed color */
    bj_mask_bg_mode  mode
);

////////////////////////////////////////////////////////////////////////////////
/// Masked blit with stretching (nearest neighbor). The mask must be 8bpp.
///
/// \param mask        The 8bpp mask bitmap (0..255 coverage).
/// \param mask_area   Optional area in the mask (0 = full mask).
/// \param dst         The destination bitmap.
/// \param dst_area    Optional destination area (0 = full destination).
/// \param fg_native   Foreground color packed for destination.
/// \param bg_native   Background color packed for destination.
/// \param mode        The background mode (see \ref bj_mask_bg_mode).
/// \return            *BJ_TRUE* if any pixel was written, *BJ_FALSE* otherwise.
///
/// \par Clipping & Mapping
///
/// The destination area is clipped to the destination bounds. The source mask
/// area is **proportionally adjusted** so that the visible sub-rectangle of
/// the stretched glyph corresponds to the same sub-rectangle of the source.
/// This avoids visual “wrap-around” artifacts when partially off-screen.
///
/// \par Coverage
///
/// Mask values are interpreted as linear coverage (0..255). Alpha blending uses
/// integer arithmetic: `mix(dst, src, a)` → `(dst*(255-a) + src*a)/255`.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_blit_mask_stretched(
    const bj_bitmap* mask,
    const bj_rect*   mask_area,     /* NULL = full mask */
    bj_bitmap*       dst,
    const bj_rect*   dst_area,      /* NULL = fill entire dst */
    uint32_t         fg_native,
    uint32_t         bg_native,
    bj_mask_bg_mode  mode
);

////////////////////////////////////////////////////////////////////////////////
/// Prints text using the default foreground color and transparent background.
///
/// \param dst        The destination bitmap.
/// \param x          X coordinate of the baseline origin.
/// \param y          Y coordinate of the baseline origin.
/// \param height     Target font height in pixels (glyphs are scaled from the
///                   internal font cell size to this height).
/// \param fg_native  Foreground color in destination-native format.
/// \param text       UTF-8/ASCII string with optional ANSI SGR sequences (see below).
///
/// \par ANSI Color Formatting
///
/// The text supports a subset of ANSI SGR sequences introduced by ESC (`\x1B`):
/// - `\x1B[0m`      Reset colors to defaults.
/// - `\x1B[30..37m` Set foreground to basic colors (black, red, green, yellow,
///                  blue, magenta, cyan, white).
/// - `\x1B[90..97m` Set foreground to bright basic colors.
/// - `\x1B[39m`     Reset foreground to the default provided in the call.
/// - `\x1B[38;2;R;G;Bm` Set truecolor foreground.
/// - Background-related codes are ignored by this function (use
///   \ref bj_blit_text for background control).
///
/// \par Behavior
///
/// The function uses the internal monochrome font mask and performs a masked
/// blit in transparent background mode (foreground over destination).
///
/// \par Clipping
///
/// Text is clipped to the destination bounds. Out-of-range glyphs are skipped.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_text(
    bj_bitmap*      dst,
    int             x,
    int             y,
    unsigned        height,
    uint32_t        fg_native,
    const char*     text
);

////////////////////////////////////////////////////////////////////////////////
/// Prints formatted text into a bitmap, similar to `printf`.
///
/// This function formats the full string using `vsnprintf` into a temporary
/// buffer (heap-allocated), then renders it with \ref bj_draw_text.
///
/// \param p_bitmap   The destination bitmap.
/// \param x          The X coordinate (top-left) where the text begins.
/// \param y          The Y coordinate (top-left) where the text begins.
/// \param height     The pixel height of the rendered font.
/// \param fg_native  Foreground color in destination-native format.
/// \param fmt        The `printf`-style format string.
/// \param ...        Additional arguments corresponding to the format string.
///
/// \par Supported formatting
/// Exactly the same as the C library `printf` (C99). All flags, width,
/// precision (including `*`), length modifiers, and conversions are supported.
///
/// \note This routine renders **foreground only**. For background or mask modes,
///       use \ref bj_blit_text (or provide a printf variant of it).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_draw_textf(
    bj_bitmap*     p_bitmap,
    int            x,
    int            y,
    unsigned       height,
    uint32_t       fg_native,
    const char*    fmt,
    ...
);

////////////////////////////////////////////////////////////////////////////////
/// Prints text with explicit foreground/background and background mode.
///
/// \param dst        The destination bitmap.
/// \param x          X coordinate of the baseline origin.
/// \param y          Y coordinate of the baseline origin.
/// \param height     Target font height in pixels.
/// \param fg_native  Foreground color in destination-native format.
/// \param bg_native  Background color in destination-native format.
/// \param mode       Background mode (see \ref bj_mask_bg_mode).
/// \param text       UTF-8/ASCII string with optional ANSI SGR sequences.
///
/// \par ANSI Color Formatting
///
/// The string may embed standard SGR sequences following `ESC '['`:
///
/// - **Reset**
///   - `\x1B[0m`        Reset both foreground and background to the defaults
///                      passed as `fg_native`/`bg_native`.
///
/// - **Basic / Bright colors**
///   - Foreground: `\x1B[30..37m` (basic), `\x1B[90..97m` (bright)
///   - Background: `\x1B[40..47m` (basic), `\x1B[100..107m` (bright)
///
/// - **Defaults**
///   - `\x1B[39m`       Reset foreground to the call’s `fg_native`.
///   - `\x1B[49m`       Reset background to the call’s `bg_native`.
///
/// - **Truecolor**
///   - Foreground: `\x1B[38;2;R;G;Bm`
///   - Background: `\x1B[48;2;R;G;Bm`
///
/// Unsupported or malformed sequences are ignored gracefully.
///
/// \par Background Modes
///
/// - `BJ_MASK_BG_TRANSPARENT`: foreground is composited where glyph coverage > 0.
/// - `BJ_MASK_BG_OPAQUE`: the glyph box is a band: mix(bg, fg, coverage).
/// - `BJ_MASK_BG_REV_TRANSPARENT`: carved-out mode where background is painted
///   with alpha = (1 - coverage) and glyph interiors remain untouched.
///
/// \par Clipping
///
/// Each glyph is pre-clipped to the destination bounds. The source mask
/// sub-rectangle is adjusted proportionally so edge glyphs render correctly
/// without wrap-around artifacts.
///
/// \par Performance
///
/// The glyph mask atlas is cached per destination bitmap. Rendering uses
/// nearest-neighbor scaling and an inner loop with integer blending.
///
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_blit_text(
    bj_bitmap*      dst,
    int             x,
    int             y,
    unsigned        height,
    uint32_t        fg_native,
    uint32_t        bg_native,
    bj_mask_bg_mode mode,            /* TRANSPARENT / OPAQUE / REV_TRANSPARENT */
    const char*     text
);

#endif
/// \} // End of bitmap group
