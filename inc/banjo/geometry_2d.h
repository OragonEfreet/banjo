////////////////////////////////////////////////////////////////////////////////
/// \file geometry_2d.h
/// 2D Geometry helpers
////////////////////////////////////////////////////////////////////////////////
/// \defgroup geometry Geometry 2D
/// \ingroup math
///
/// \brief 2D Geometry utilities
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_GEOMETRY_2D_H
#define BJ_GEOMETRY_2D_H

#include <banjo/api.h>
#include <banjo/vec.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Check if two line segments intersect.
///
/// Tests whether the line segment from \p a0 to \p a1 intersects with the
/// segment from \p b0 to \p b1 in 2D space.
///
/// \param a0 Start point of first segment.
/// \param a1 End point of first segment.
/// \param b0 Start point of second segment.
/// \param b1 End point of second segment.
///
/// \return BJ_TRUE if the segments intersect, BJ_FALSE otherwise.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_check_segments_hit(
    struct bj_vec2 a0,
    struct bj_vec2 a1,
    struct bj_vec2 b0,
    struct bj_vec2 b1
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Check if a circle intersects with a line segment.
///
/// Tests whether a circle centered at \p c with radius \p r intersects
/// the line segment from \p l0 to \p l1.
///
/// \param c  Circle center position.
/// \param r  Circle radius.
/// \param l0 Start point of the line segment.
/// \param l1 End point of the line segment.
///
/// \return BJ_TRUE if the circle intersects the segment, BJ_FALSE otherwise.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_check_circle_segment_hit(
    struct bj_vec2 c,
    bj_real r,
    struct bj_vec2 l0,
    struct bj_vec2 l1
);


////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_GEOMETRY_2D_H */
