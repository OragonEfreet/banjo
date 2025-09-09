#ifndef BJ_PHYSICS_H
#define BJ_PHYSICS_H

#include <banjo/math.h>
#include <banjo/vec.h>

////////////////////////////////////////////////////////////////////////////////
/// \file physics.h
/// Physics helpers (SI units, but dimensionally consistent with any unit system).
////////////////////////////////////////////////////////////////////////////////
/// \defgroup physics Physics
/// \ingroup core
///
/// \brief Basic physics utilities (constant-acceleration kinematics, etc.)
///
/// This header provides small helpers for common physics operations. By default,
/// quantities are interpreted in **SI units** (meters, seconds), but formulas are
/// **dimensionally homogeneous**: results are correct for **any consistent unit
/// system** (e.g., km and s; cm and s), provided all inputs use the same system.
///
/// Dimensionality uses the base dimensions
/// **L** (length) and **T** (time). For example:
/// - position: [L]
/// - velocity: [L T^-1]
/// - acceleration: [L T^-2]
/// - time: [T]
///
/// \{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes position at time \a t under constant acceleration.
///
/// Uses the standard constant-acceleration kinematics formula:
/// `x = 0.5 * a * t^2 + v0 * t + x0`
///
/// \param position     Initial position [L]
/// \param velocity     Initial velocity [L T^-1]
/// \param acceleration Constant acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Position at time \a t, in [L] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_kinematics(
    bj_real position,
    bj_real velocity,
    bj_real acceleration,
    bj_real time
) {
    return BJ_F(0.5) * acceleration * time * time + velocity * time + position;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes velocity at time \a t under constant acceleration.
///
/// Uses: `v(t) = a * t + v0`
///
/// \param velocity     Initial velocity [L T^-1]
/// \param acceleration Constant acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Velocity at time \a t, in [L T^-1].
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE bj_real bj_kinematics_velocity(
    bj_real velocity,
    bj_real acceleration,
    bj_real time
) {
    return acceleration * time + velocity;
}

/// \}

#endif
