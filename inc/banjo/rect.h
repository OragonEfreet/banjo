////////////////////////////////////////////////////////////////////////////////
/// \file rect.h
/// Defines the struct bj_rect struct representing a rectangle with position and dimensions.
////////////////////////////////////////////////////////////////////////////////
/// \addtogroup math
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_RECT_H
#define BJ_RECT_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedef for  bj_rect
struct bj_rect;

////////////////////////////////////////////////////////////////////////////////
/// Represents a rectangle with position and dimensions.
struct bj_rect {
    int16_t  x; ///< The x-coordinate of the rectangle's top-left corner.
    int16_t  y; ///< The y-coordinate of the rectangle's top-left corner.
    uint16_t w; ///< The width of the rectangle.
    uint16_t h; ///< The height of the rectangle.
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the intersection of two  bj_rect.
///
/// \param rect_a Pointer to the first rectangle. Must not be *0*.
/// \param rect_b Pointer to the second rectangle. Must not be *0*.
/// \param result Pointer to the rectangle where the result will be stored.
///               Can be *0* if only checking intersection presence.
///
/// \return *BJ_TRUE* if the rectangles intersect and neither input is *0*, *BJ_FALSE*.
///
/// \note Both input rectangles must be valid and properly initialized. If either
///       `rect_a` or `rect_b` is *0*, the function returns *0*. If `result`
///       is *0*, the function only checks for intersection presence and does not
///       compute the intersection rectangle.
///
/// Example usage:
/// \code
/// struct bj_rect rect_a = {0, 0, 10, 10};
/// struct bj_rect rect_b = {5, 5, 15, 15};
/// struct bj_rect result;
/// if (bj_rect_intersection(&rect_a, &rect_b, 0)) {
///     // There is an intersection between rect_a and rect_b.
/// }
/// \endcode
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_rect_intersection(
    const struct bj_rect* rect_a,
    const struct bj_rect* rect_b,
    struct bj_rect*       result
);

#endif

/// \} // End of rect group

