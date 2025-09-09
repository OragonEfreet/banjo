////////////////////////////////////////////////////////////////////////////////
/// \file physics_3d.h
/// Physics helpers (SI units, but dimensionally consistent with any unit system).
////////////////////////////////////////////////////////////////////////////////
/// \defgroup physics_3d 3D Physics
/// \ingroup physics
///
/// \brief 3D physics utilities (particles, forces, kinematics).
///
/// This header provides small helpers for common 3D physics operations. By default,
/// quantities are interpreted in **SI units** (meters, seconds), but formulas are
/// **dimensionally homogeneous**: results are correct for **any consistent unit
/// system** (e.g., km and s; cm and s), provided all inputs use the same system.
///
/// Dimensionality uses the base dimensions **L** (length) and **T** (time). For example:
/// - position: [L]
/// - velocity: [L T^-1]
/// - acceleration: [L T^-2]
/// - time: [T]
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_PHYSICS_3D_H
#define BJ_PHYSICS_3D_H

#include <banjo/api.h>
#include <banjo/math.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief 3D particle state and physical properties.
///
/// Positions, velocities, accelerations and force accumulator are expressed in
/// world space. Damping is a unitless velocity decay factor applied per-step.
/// Mass is represented as inverse mass; use 0 to represent an immovable object.
///
/// \note All vectors use the engine’s right-handed world basis.
/// \see bj_accumulate_world_gravity_3d, bj_accumulate_drag_3d
///
/// \param position     Current 3D position [L]
/// \param velocity     Current 3D velocity [L T^-1]
/// \param acceleration Current 3D acceleration [L T^-2]
/// \param forces       Accumulated force for the next integration step [M L T^-2]
/// \param damping      Velocity damping factor in [0, 1]
/// \param inverse_mass Inverse of mass [M^-1]; 0 ⇒ infinite mass
////////////////////////////////////////////////////////////////////////////////
struct bj_particle_3d_t {
    bj_vec3 position;
    bj_vec3 velocity;
    bj_vec3 acceleration;
    bj_vec3 forces;
    bj_real damping;
    bj_real inverse_mass;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Shorthand alias for \ref bj_particle_3d_t.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_particle_3d_t bj_particle_3d;

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the scalar drag coefficient for a 3D velocity.
///
/// Quadratic drag model:
/// `c = k1 * |v| + k2 * |v|^2`
/// so that the drag force is `F_d = -c * \hat{v}`.
///
/// \param vel  Velocity vector v [L T^-1]
/// \param k1   Linear drag constant [M T^-1]
/// \param k2   Quadratic drag constant [M L^-1]
///
/// \return Drag coefficient \a c >= 0 in [M T^-1].
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_real bj_particle_drag_coefficient_3d(
    const bj_vec3 vel,
    const bj_real k1,
    const bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the 3D drag force for a velocity and coefficients.
///
/// Uses the same model as \ref bj_particle_drag_coefficient_3d and writes:
/// `result = -c * \hat{v}`, with `c = k1 * |v| + k2 * |v|^2`.
///
/// \param result Output force vector F_d [M L T^-2]
/// \param vel    Velocity vector v [L T^-1]
/// \param k1     Linear drag constant [M T^-1]
/// \param k2     Quadratic drag constant [M L^-1]
///
/// \return BJ_TRUE on success; BJ_FALSE if |v| == 0 (force set to {0,0,0}).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_particle_drag_force_3d(
    bj_real result[BJ_RESTRICT static 3],
    const bj_real vel[BJ_RESTRICT static 3],
    const bj_real k1,
    const bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Accumulates uniform world gravity into the particle’s force buffer.
///
/// Adds gravitational force for the next integration step:
/// `F_g = m * g * d`, where \a g is the scalar gravity magnitude [L T^-2],
/// \a m is mass (= 1 / inverse_mass), and \a d is the engine’s world-down
/// unit vector.
///
/// \param p_particle Particle to modify
/// \param gravity    Gravity magnitude [L T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_accumulate_world_gravity_3d(
    bj_particle_3d* p_particle,
    bj_real         gravity
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Accumulates aerodynamic drag into the particle’s force buffer.
///
/// Computes `F_d` with \ref bj_particle_drag_force_3d using the current
/// particle velocity, then adds it to \c p_particle->forces.
///
/// \param p_particle Particle to modify
/// \param k1         Linear drag constant [M T^-1]
/// \param k2         Quadratic drag constant [M L^-1]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_accumulate_drag_3d(
    bj_particle_3d* p_particle,
    bj_real k1,
    bj_real k2
);

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
BANJO_EXPORT void bj_kinematics_3d(
    bj_real       out[BJ_RESTRICT static 3],
    const bj_real position[BJ_RESTRICT static 3],
    const bj_real velocity[BJ_RESTRICT static 3],
    const bj_real acceleration[BJ_RESTRICT static 3],
    bj_real time
);

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
BANJO_EXPORT void bj_kinematics_velocity_3d(
    bj_real              out[ BJ_RESTRICT static 3 ],
    const bj_real        velocity[ BJ_RESTRICT static 3 ],
    const bj_real        acceleration[ BJ_RESTRICT static 3 ],
    bj_real              time
);

////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_PHYSICS_3D_H */
