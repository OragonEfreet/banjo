////////////////////////////////////////////////////////////////////////////////
/// \file
/// Defines the bj_rect struct representing a rectangle with position and dimensions.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup rect Rectangle
/// \ingroup graphics
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for \ref bj_rect_t
typedef struct bj_rect_t bj_rect;

////////////////////////////////////////////////////////////////////////////////
/// Represents a rectangle with position and dimensions.
struct bj_rect_t {
    i16 x; ///< The x-coordinate of the rectangle's top-left corner.
    i16 y; ///< The y-coordinate of the rectangle's top-left corner.
    u16 w; ///< The width of the rectangle.
    u16 h; ///< The height of the rectangle.
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the intersection of two \ref bj_rect.
///
/// \param p_rect_a Pointer to the first rectangle. Must not be *0*.
/// \param p_rect_b Pointer to the second rectangle. Must not be *0*.
/// \param p_result Pointer to the rectangle where the result will be stored. 
///                 Can be *0* if only checking intersection presence.
///
/// \return Non-zero if the rectangles intersect and neither input is *0*, *0* otherwise.
///
/// \note Both input rectangles must be valid and properly initialized. If either 
///       `p_rect_a` or `p_rect_b` is *0*, the function returns *0*. If \p p_result
///       is *0*, the function only checks for intersection presence and does not 
///       compute the intersection rectangle.
///
/// Example usage:
/// \code
/// bj_rect rect_a = {0, 0, 10, 10};
/// bj_rect rect_b = {5, 5, 15, 15};
/// bj_rect result;
/// if (bj_rect_intersect(&rect_a, &rect_b, 0)) {
///     // There is an intersection between rect_a and rect_b.
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT int bj_rect_intersect(
    const bj_rect* p_rect_a,
    const bj_rect* p_rect_b,
    bj_rect*       p_result
);

/// \} // End of rect group

