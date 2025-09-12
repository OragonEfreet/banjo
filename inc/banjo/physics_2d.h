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

BANJO_EXPORT void bj_kinematics_2d(
    bj_real       out[BJ_RESTRICT static 2],
    const bj_real position[BJ_RESTRICT static 2],
    const bj_real velocity[BJ_RESTRICT static 2],
    const bj_real acceleration[BJ_RESTRICT static 2],
    bj_real time
);

BANJO_EXPORT void bj_kinematics_velocity_2d(
    bj_real              out[ BJ_RESTRICT static 2 ],
    const bj_real        velocity[ BJ_RESTRICT static 2 ],
    const bj_real        acceleration[ BJ_RESTRICT static 2 ],
    bj_real              time
);

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
typedef struct bj_point_mass_2d_t bj_point_mass_2d;

BANJO_EXPORT void bj_point_mass_add_force_2d(
    bj_point_mass_2d* p_point_mass,
    const bj_vec2 force
);

BANJO_EXPORT void bj_step_point_mass_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real dt
);

BANJO_EXPORT void bj_apply_gravity_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real           gravity
);

BANJO_EXPORT void bj_apply_point_gravity_2d(
    bj_point_mass_2d* BJ_RESTRICT       p_point_mass_from,
    const bj_point_mass_2d* BJ_RESTRICT p_point_mass_to,
    const bj_real                     gravity_factor
);

BANJO_EXPORT void bj_apply_point_gravity_softened_2d(
    bj_point_mass_2d*       BJ_RESTRICT p_point_mass_from,
    const bj_point_mass_2d* BJ_RESTRICT p_point_mass_to,
    const bj_real                       gravity_factor,
    const bj_real                       epsilon
);

BANJO_EXPORT void bj_apply_drag_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real k1,
    bj_real k2
);

BANJO_EXPORT bj_real bj_point_mass_drag_coefficient_2d(
    const bj_vec2 vel,
    const bj_real k1,
    const bj_real k2
);

BANJO_EXPORT bj_bool bj_point_mass_drag_force_2d(
    bj_real result[BJ_RESTRICT static 2],
    const bj_real vel[BJ_RESTRICT static 2],
    const bj_real k1,
    const bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// Angular
////////////////////////////////////////////////////////////////////////////////
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

BANJO_EXPORT void bj_step_angular_2d(
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

BANJO_EXPORT void bj_step_rigid_body_2d(
    bj_rigid_body_2d* body,
    double            delta_time
);

////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_PHYSICS_2D_H */
