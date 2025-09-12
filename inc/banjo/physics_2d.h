////////////////////////////////////////////////////////////////////////////////
/// \file physics_2d.h
/// Physics helpers (SI units, but dimensionally consistent with any unit system).
////////////////////////////////////////////////////////////////////////////////
/// \defgroup physics_2d 2D Physics
/// \ingroup physics
///
/// \brief 2D physics utilities (point masses, forces, kinematics).
///
/// This header provides small helpers for common 2D physics operations. By default,
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
#ifndef BJ_PHYSICS_2D_H
#define BJ_PHYSICS_2D_H

#include <banjo/api.h>
#include <banjo/math.h>
#include <banjo/vec.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief 2D point mass state and physical properties.
///
/// Positions, velocities, accelerations and force accumulator are expressed in
/// world space. Damping is a unitless velocity decay factor applied per-step.
/// Mass is represented as inverse mass; use 0 to represent an immovable object.
///
/// \param position     Current 2D position [L]
/// \param velocity     Current 2D velocity [L T^-1]
/// \param acceleration Current 2D acceleration [L T^-2]
/// \param forces       Accumulated force for the next integration step [M L T^-2]
/// \param damping      Velocity damping factor in [0, 1]
/// \param inverse_mass Inverse of mass [M^-1]; 0 ⇒ infinite mass
////////////////////////////////////////////////////////////////////////////////
struct bj_point_mass_2d_t {
    bj_vec2 position;
    bj_vec2 velocity;
    bj_vec2 acceleration;
    bj_vec2 forces;
    bj_real damping;
    bj_real inverse_mass;
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Shorthand alias for \ref bj_point_mass_2d_t.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_point_mass_2d_t bj_point_mass_2d;

BANJO_EXPORT void bj_point_mass_set_mass_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real         mass
);

BANJO_EXPORT void bj_point_mass_add_force_2d(
    bj_point_mass_2d* p_point_mass,
    const bj_vec2 force
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Integrates point mass state forward by \a dt seconds.
///
/// Advances velocity and position using current acceleration and accumulated
/// forces. Applies damping to velocity. Intended for fixed-timestep updates.
///
/// \param p_point_mass Particle to integrate
/// \param dt         Time step [T]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_integrate_point_mass_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real dt
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Accumulates uniform world gravity into the point mass’ force buffer.
///
/// Adds gravitational force for the next integration step:
/// `F_g = m * g * d`, where \a g is the scalar gravity magnitude [L T^-2],
/// \a m is mass (= 1 / inverse_mass), and \a d is the engine’s world-down
/// unit vector.
///
/// \param p_point_mass Particle to modify
/// \param gravity    Gravity magnitude [L T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_accumulate_world_gravity_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real         gravity
);

BANJO_EXPORT void bj_accumulate_point_gravity_2d(
    bj_point_mass_2d* BJ_RESTRICT       p_point_mass_from,
    const bj_point_mass_2d* BJ_RESTRICT p_point_mass_to,
    const bj_real                     gravity_factor
);

BANJO_EXPORT void bj_accumulate_point_gravity_softened_2d(
    bj_point_mass_2d*       BJ_RESTRICT p_point_mass_from,
    const bj_point_mass_2d* BJ_RESTRICT p_point_mass_to,
    const bj_real                     gravity_factor,
    const bj_real                     epsilon
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Accumulates aerodynamic drag into the point mass’ force buffer.
///
/// Computes 2D drag based on current point mass velocity and adds it to
/// \c p_point_mass->forces. See \ref bj_point_mass_drag_force_2d.
///
/// \param p_point_mass Particle to modify
/// \param k1         Linear drag constant [M T^-1]
/// \param k2         Quadratic drag constant [M L^-1]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_accumulate_drag_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real k1,
    bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the scalar drag coefficient for a 2D velocity.
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
BANJO_EXPORT bj_real bj_point_mass_drag_coefficient_2d(
    const bj_vec2 vel,
    const bj_real k1,
    const bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Computes the 2D drag force for a velocity and coefficients.
///
/// Uses the same model as \ref bj_point_mass_drag_coefficient_2d and writes:
/// `result = -c * \hat{v}`, with `c = k1 * |v| + k2 * |v|^2`.
///
/// \param result Output force vector F_d [M L T^-2]
/// \param vel    Velocity vector v [L T^-1]
/// \param k1     Linear drag constant [M T^-1]
/// \param k2     Quadratic drag constant [M L^-1]
///
/// \return BJ_TRUE on success; BJ_FALSE if |v| == 0 (force set to {0,0}).
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_point_mass_drag_force_2d(
    bj_real result[BJ_RESTRICT static 2],
    const bj_real vel[BJ_RESTRICT static 2],
    const bj_real k1,
    const bj_real k2
);

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
BANJO_EXPORT void bj_kinematics_2d(
    bj_real       out[BJ_RESTRICT static 2],
    const bj_real position[BJ_RESTRICT static 2],
    const bj_real velocity[BJ_RESTRICT static 2],
    const bj_real acceleration[BJ_RESTRICT static 2],
    bj_real time
);

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
BANJO_EXPORT void bj_kinematics_velocity_2d(
    bj_real              out[ BJ_RESTRICT static 2 ],
    const bj_real        velocity[ BJ_RESTRICT static 2 ],
    const bj_real        acceleration[ BJ_RESTRICT static 2 ],
    bj_real              time
);

struct bj_angular_2d_t {
    bj_real value;
    bj_real velocity;
    bj_real acceleration;
    bj_real torque;
    bj_real damping;
    bj_real inverse_inertia;
};
typedef struct bj_angular_2d_t bj_angular_2d;

BANJO_EXPORT void bj_add_angular_torque_2d(
    bj_angular_2d* angular,
    bj_real torque
);

BANJO_EXPORT void bj_integrate_angular_2d(
    bj_angular_2d* angular,
    double         delta_time
);

struct bj_rigid_body_2d_t {
    struct bj_point_mass_2d_t point_mass;
    struct bj_angular_2d_t    angular;
};

typedef struct bj_rigid_body_2d_t bj_rigid_body_2d;

BANJO_EXPORT void bj_rigid_body_add_force_2d(
    bj_rigid_body_2d* body,
    const bj_vec2 force
);

BANJO_EXPORT void bj_integrate_rigid_body_2d(
    bj_angular_2d* angular,
    double         delta_time
);



////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_PHYSICS_2D_H */
