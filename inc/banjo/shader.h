////////////////////////////////////////////////////////////////////////////////
/// \file shader.h
/// \brief Basic shader-like bitmap manipulation
///
////////////////////////////////////////////////////////////////////////////////
/// \defgroup shaders Shaders
/// \ingroup graphics
///
/// Software shader-like API for \ref bj_bitmap.
///
/// This API provides facilities to manipulate the pixels of a \ref bj_bitmap
/// object in a similar way GPU shaders do.
///
/// A shader function is any user-provided function that corresponds to the
/// \ref bj_bitmap_shading_fn_t signature.
/// Such function is passed to \ref bj_bitmap_apply_shader which calls the
/// shader fonction on every pixel of the bitmap.
///
/// This header file also provides some math functions usually found in
/// most shader languages, such as \ref bj_step and \ref bj_smoothstep.
/// Other useful math-related functions can be found in \ref math.h.
///
/// \{
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/math.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Function type for a bitmap shading operation.
///
/// A shader function pointer is provided to \ref bj_bitmap_apply_shader and
/// will be called for each pixel of the provided bitmap.
///
/// \param out_color   Pointer to the output color (in linear RGB space).
/// \param pixel_coord Position of the pixel in 2D coordinates.
/// \param user_data   Optional user data passed through from 
///                    \ref bj_bitmap_apply_shader.
/// \return _1_ on success, non-zero if the pixel should be skipped or discarded.
///
////////////////////////////////////////////////////////////////////////////////
typedef int (*bj_bitmap_shading_fn_t)(bj_vec3 out_color, const bj_vec2 pixel_coord, void* user_data);

////////////////////////////////////////////////////////////////////////////////
/// \brief Clamps a float between a minimum and a maximum value.
///
/// \param  x The input value.
/// \param  min The minimum value allowed.
/// \param  max The maximum value allowed.
/// \return The clamped value.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT float bj_clamp(
    float x,
    float min,
    float max
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns _0.0_ if `x < edge`, else _1.0_.
///
/// Often used for binary thresholding in shaders.
///
/// \param edge The threshold edge.
/// \param x The input value.
/// \return _0.0_ or _1.0_ depending on x's relationship to edge.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT float bj_step(
    float edge,
    float x
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Performs smooth Hermite interpolation between 0 and 1 over a range.
///
/// \param edge0 The lower bound of the transition.
/// \param edge1 The upper bound of the transition.
/// \param x The input value.
/// \return The smoothed value in [0, 1].
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT float bj_smoothstep(
    float edge0,
    float edge1,
    float x
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Returns the fractional part of a float.
///
/// \param x The input value.
/// \return The fractional part (x - floor(x)).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT float bj_fract(
    float x
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes a floor-style modulus between two floats.
///
/// \param x The dividend.
/// \param y The divisor.
/// \return The result of x mod y, using floor-based logic.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_mod(
    float x,
    float y
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Shader input control flags
///
/// These flags are passed to \ref bj_bitmap_apply_shader to control how
/// the inputs to the shader function (\ref bj_bitmap_shading_fn_t) are transformed.
/// They affect how the pixel coordinates are interpreted and how the shader's
/// output color is handled.
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_shader_flag_t {

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Invert the X coordinate of the input pixel.
    ///
    /// By default, the coordinate system matches that of \ref bj_bitmap.
    /// The origin is at the top-left corner, with X increasing to the right
    /// and Y increasing downward, ranging from 0 to the bitmap's width and height.
    ///
    /// When this flag is set, the X coordinate is mirrored as if the bitmap
    /// were flipped horizontally.
    ///
    /// This flag can be combined with \ref BJ_SHADER_NORMALIZE_COORDS and
    /// \ref BJ_SHADER_CENTER_COORDS. It is applied *after* those transformations.
    ////////////////////////////////////////////////////////////////////////////////
    BJ_SHADER_INVERT_X         = 0x01,

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Invert the Y coordinate of the input pixel.
    ///
    /// By default, the coordinate system matches that of \ref bj_bitmap.
    /// The origin is at the top-left corner, with X increasing to the right
    /// and Y increasing downward, ranging from 0 to the bitmap's width and height.
    ///
    /// When this flag is set, the Y coordinate is mirrored as if the bitmap
    /// were flipped vertically.
    ///
    /// This flag can be combined with \ref BJ_SHADER_NORMALIZE_COORDS and
    /// \ref BJ_SHADER_CENTER_COORDS. It is applied *after* those transformations.
    ////////////////////////////////////////////////////////////////////////////////
    BJ_SHADER_INVERT_Y         = 0x02,

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Clamp the output color to the range _[0.0, 1.0]_.
    ///
    /// The shader function is expected to output RGB components in the range _[0.0, 1.0]_.
    /// If this flag is set, the output color will automatically be clamped to
    /// that range, preventing overflow or underflow.
    ///
    /// This can help avoid artifacts if the shader generates values outside the valid range.
    ////////////////////////////////////////////////////////////////////////////////
    BJ_SHADER_CLAMP_COLOR      = 0x04,

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Normalize pixel coordinates to the _[0.0, 1.0]_ range.
    ///
    /// Converts the pixel's X and Y coordinates into normalized values between
    /// 0.0 and 1.0, based on the width and height of the bitmap.
    ///
    /// This transformation maintains the original orientation (top-left origin),
    /// unless combined with \ref BJ_SHADER_INVERT_X or \ref BJ_SHADER_INVERT_Y.
    ///
    /// If used together with \ref BJ_SHADER_CENTER_COORDS, the final coordinate space
    /// becomes _[-1.0, 1.0]_, centered around the origin.
    ////////////////////////////////////////////////////////////////////////////////
    BJ_SHADER_NORMALIZE_COORDS = 0x08,

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Center pixel coordinates around the origin.
    ///
    /// Translates the coordinate system so that (0,0) represents the center of the bitmap.
    ///
    /// By default, pixel coordinates range from (0,0) to (width, height).
    /// When this flag is enabled:
    ///
    /// - If used **without** \ref BJ_SHADER_NORMALIZE_COORDS, 
    ///   coordinates are shifted into the range:
    ///   - X: _[-width/2, width/2]_
    ///   - Y: _[-height/2, height/2]_
    ///
    /// - If used **with** \ref BJ_SHADER_NORMALIZE_COORDS,
    ///   coordinates are shifted into the range:
    ///   - X, Y ∈ _[-1.0, 1.0]_
    ///
    /// This transformation is applied before coordinate inversion flags.
    ////////////////////////////////////////////////////////////////////////////////
    BJ_SHADER_CENTER_COORDS    = 0x10,

} bj_shader_flag;

////////////////////////////////////////////////////////////////////////////////
/// Flagset alias for \ref bj_bitmap_apply_shader.
///
/// This flagset value can be passed to \ref bj_bitmap_apply_shader and
/// corresponds to the most commonly used flags:
///
/// - Invert Y (orient from bottom-left corner),
/// - Clamp color between 0.0 and 1.0,
/// - Convert pixel coordinates in the _[-1.0 ; 1.0]_ space
////////////////////////////////////////////////////////////////////////////////
#define BJ_SHADER_STANDARD_FLAGS (BJ_SHADER_INVERT_Y | BJ_SHADER_CLAMP_COLOR | BJ_SHADER_NORMALIZE_COORDS | BJ_SHADER_CENTER_COORDS)

////////////////////////////////////////////////////////////////////////////////
/// \brief Applies a shader function to every pixel in a bitmap.
///
/// The shader function is called per pixel, with access to the 2D coordinate and
/// a reference to output a linear RGB color. Optional user data and behavior flags
/// can be provided.
///
/// \param p_bitmap Pointer to the target bitmap to be modified.
/// \param p_shader A pointer to the shader function to call per pixel.
/// \param p_data User-defined data passed to each shader call.
/// \param flags Combination of \ref bj_shader_flag controlling coordinate
///        and color behavior.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_bitmap_apply_shader(
    bj_bitmap*             p_bitmap,
    bj_bitmap_shading_fn_t p_shader,
    void*                  p_data,
    uint8_t                flags
);

/// \} // End of bitmap group
