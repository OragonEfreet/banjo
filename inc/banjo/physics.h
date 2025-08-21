#ifndef BJ_PHYSICS_H
#define BJ_PHYSICS_H

#include <banjo/math.h>

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
/// `x(t) = 0.5 * a * t^2 + v0 * t + x0`
///
/// \param acceleration Constant acceleration in meters per second squared (m/s^2) in SI,
///                     or [L T^-2] in any consistent unit system.
/// \param velocity     Initial velocity in meters per second (m/s) in SI,
///                     or [L T^-1] in any consistent unit system.
/// \param position     Initial position in meters (m) in SI,
///                     or [L] in any consistent unit system.
/// \param time         Elapsed time in seconds (s) in SI,
///                     or [T] in any consistent unit system.
///
/// \return Position at time \a t, in meters (m) in SI, or [L] in the chosen system.
///
/// \note Dimensional consistency: each term has dimension [L].
///       (0.5 * a * t^2 → [L T^-2] * [T^2] = [L]; v0 * t → [L T^-1] * [T] = [L].)
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_kinematics_position(
    bj_real acceleration,
    bj_real velocity,
    bj_real position,
    bj_real time
) {
    return BJ_F(0.5) * acceleration * time * time + velocity * time + position;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes velocity at time \a t under constant acceleration.
///
/// Uses the standard constant-acceleration kinematics formula:
/// `v(t) = a * t + v0`
///
/// \param acceleration Constant acceleration in meters per second squared (m/s^2) in SI,
///                     or [L T^-2] in any consistent unit system.
/// \param velocity     Initial velocity in meters per second (m/s) in SI,
///                     or [L T^-1] in any consistent unit system.
/// \param time         Elapsed time in seconds (s) in SI,
///                     or [T] in any consistent unit system.
///
/// \return Velocity at time \a t, in meters per second (m/s) in SI,
///         or [L T^-1] in the chosen system.
///
/// \note Dimensional consistency: a * t → [L T^-2] * [T] = [L T^-1].
////////////////////////////////////////////////////////////////////////////////
static inline bj_real bj_kinematics_velocity(
    bj_real acceleration,
    bj_real velocity,
    bj_real time
) {
    return acceleration * time + velocity;
}

/// \}

#endif
