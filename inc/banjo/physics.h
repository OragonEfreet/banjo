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
/// \brief Computes positions at time \a t under given constant accelerations.
///
/// Uses the standard constant-acceleration kinematics formula, where for each
/// index i:
/// `x[i] = 0.5 * a[i] * t[i]^2 + v0[i] * t + x0[i]`
///
/// \param n            Number of positions to compute
/// \param out          A pointer to the _n_ resulting positions
/// \param position     A pointer to the _n_ initial positions [L]
/// \param velocity     A pointer to the _n_ initial velocities [L T^-1]
/// \param acceleration A pointer to the _n_ constant accelerations [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Position at time \a t, in [L] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_kinematics_array(
    size_t                     n,
    bj_real*       BJ_RESTRICT out,
    const bj_real* BJ_RESTRICT position,
    const bj_real* BJ_RESTRICT velocity,
    const bj_real* BJ_RESTRICT acceleration,
    bj_real        time
) {
    for(size_t i = 0 ; i < n ; ++i) {
        out[i] = bj_kinematics(position[i], velocity[i], acceleration[i], time);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes 2D positions at time \a t under given constant accelerations.
///
/// Uses the standard constant-acceleration kinematics formula, where for each
/// index i:
/// `x[i] = 0.5 * a[i] * t[i]^2 + vx0[i] * t + x0[i]`
/// `y[i] = 0.5 * a[i] * t[i]^2 + vy0[i] * t + y0[i]`
///
/// \param position     Initial 2D position [L]
/// \param velocity     Initial 2D velocity [L T^-1]
/// \param acceleration Constant 2D acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Position at time \a t, in [L] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_kinematics_2d(
    bj_real       out[BJ_RESTRICT static 2],
    const bj_real position[BJ_RESTRICT static 2],
    const bj_real velocity[BJ_RESTRICT static 2],
    const bj_real acceleration[BJ_RESTRICT static 2],
    bj_real time
) {
    bj_kinematics_array(2, out, position, velocity, acceleration, time);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes 3D positions at time \a t under given constant accelerations.
///
/// Uses the standard constant-acceleration kinematics formula, where for each
/// index i:
/// `x[i] = 0.5 * a[i] * t[i]^2 + vx0[i] * t + x0[i]`
/// `y[i] = 0.5 * a[i] * t[i]^2 + vy0[i] * t + y0[i]`
/// `z[i] = 0.5 * a[i] * t[i]^2 + vz0[i] * t + z0[i]`
///
/// \param position     Initial 3D position [L]
/// \param velocity     Initial 3D velocity [L T^-1]
/// \param acceleration Constant 3D acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Position at time \a t, in [L] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_kinematics_3d(
    bj_real       out[BJ_RESTRICT static 3],
    const bj_real position[BJ_RESTRICT static 3],
    const bj_real velocity[BJ_RESTRICT static 3],
    const bj_real acceleration[BJ_RESTRICT static 3],
    bj_real time
) {
    bj_kinematics_array(3, out, position, velocity, acceleration, time);
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

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes velocities at time \a t under given constant accelerations.
///
/// Uses the standard constant-acceleration kinematics formula, where for each
/// index i:
/// `v[i] = a[i] * t + v0[i]`
///
/// \param n            Number of velocities to compute
/// \param out          A pointer to the _n_ resulting velocities [L T^-1]
/// \param velocity     A pointer to the _n_ initial velocities [L T^-1]
/// \param acceleration A pointer to the _n_ constant accelerations [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return Velocity at time \a t, in [L T^-1] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_kinematics_velocity_array(
    size_t                     n,
    bj_real*       BJ_RESTRICT out,
    const bj_real* BJ_RESTRICT velocity,
    const bj_real* BJ_RESTRICT acceleration,
    bj_real                    time
) {
    for (size_t i = 0; i < n; ++i) {
        out[i] = bj_kinematics_velocity(velocity[i], acceleration[i], time);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes 2D velocity at time \a t under constant acceleration.
///
/// Uses the standard constant-acceleration kinematics formula, applied
/// component-wise:
/// `vx(t) = ax * t + vx0`
/// `vy(t) = ay * t + vy0`
///
/// \param velocity     Initial 2D velocity [L T^-1]
/// \param acceleration Constant 2D acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return 2D velocity at time \a t, in [L T^-1] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_kinematics_velocity_2d(
    bj_real              out[ BJ_RESTRICT static 2 ],
    const bj_real        velocity[ BJ_RESTRICT static 2 ],
    const bj_real        acceleration[ BJ_RESTRICT static 2 ],
    bj_real              time
) {
    bj_kinematics_velocity_array(2, out, velocity, acceleration, time);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes 3D velocity at time \a t under constant acceleration.
///
/// Uses the standard constant-acceleration kinematics formula, applied
/// component-wise:
/// `vx(t) = ax * t + vx0`
/// `vy(t) = ay * t + vy0`
/// `vz(t) = az * t + vz0`
///
/// \param velocity     Initial 3D velocity [L T^-1]
/// \param acceleration Constant 3D acceleration [L T^-2]
/// \param time         Elapsed time [T]
///
/// \return 3D velocity at time \a t, in [L T^-1] in the chosen system.
////////////////////////////////////////////////////////////////////////////////
static BJ_INLINE void bj_kinematics_velocity_3d(
    bj_real              out[ BJ_RESTRICT static 3 ],
    const bj_real        velocity[ BJ_RESTRICT static 3 ],
    const bj_real        acceleration[ BJ_RESTRICT static 3 ],
    bj_real              time
) {
    bj_kinematics_velocity_array(3, out, velocity, acceleration, time);
}


/// \}

#endif
