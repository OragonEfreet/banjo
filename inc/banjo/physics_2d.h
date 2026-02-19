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
/// Dimensionality uses the base dimensions L (length) and T (time). For example:
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
/// \brief Integrate constant-acceleration 2D kinematics: position at time t.
///
/// Uses: out = position + velocity * time + 0.5 * acceleration * time^2.
///
/// \param position      Initial position [L]
/// \param velocity      Initial velocity [L T^-1]
/// \param acceleration  Constant acceleration [L T^-2]
/// \param time          Elapsed time t [T]
/// \return  Output position at time t [L]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_vec2 bj_compute_kinematics_2d(
    struct bj_vec2  position,
    struct bj_vec2  velocity,
    struct bj_vec2  acceleration,
    bj_real  time
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Integrate constant-acceleration 2D kinematics: velocity at time t.
///
/// Uses: out = velocity + acceleration * time.
///
/// \param velocity      Initial velocity [L T^-1]
/// \param acceleration  Constant acceleration [L T^-2]
/// \param time          Elapsed time t [T]
/// \return Output velocity at time t [L T^-1]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_vec2 bj_compute_kinematics_velocity_2d(
    struct bj_vec2 velocity,
    struct bj_vec2 acceleration,
    bj_real time
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
/// \param inverse_mass Inverse of mass [M^-1]; 0 means infinite mass
////////////////////////////////////////////////////////////////////////////////
struct bj_particle_2d {
    struct bj_vec2 position;
    struct bj_vec2 velocity;
    struct bj_vec2 acceleration;
    struct bj_vec2 forces;
    bj_real damping;
    bj_real inverse_mass;
};
struct bj_particle_2d;

////////////////////////////////////////////////////////////////////////////////
/// \brief Add a force to a particle's accumulator.
///
/// The force will be used on the next integration step, then should be cleared
/// by the step routine.
///
/// \param particle Particle to modify
/// \param force      Force to add [M L T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_particle_force_2d(
    struct bj_particle_2d* particle,
    const struct bj_vec2 force
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Semi-implicit Euler step for a particle.
///
/// Integrates acceleration and velocity, applies damping, and advances position.
/// Clears the force accumulator after use.
///
/// \param particle Particle to integrate
/// \param dt         Time step [T]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_step_particle_2d(
    struct bj_particle_2d* particle,
    double dt
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Apply constant downward gravity in world space to a particle.
///
/// Adds force F = m * g * (0, -1) assuming -Y is "down".
///
/// \param particle Particle to affect
/// \param gravity    Gravitational acceleration magnitude g >= 0 [L T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_gravity_2d(
    struct bj_particle_2d* particle,
    bj_real gravity
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Apply point gravity from one particle to another.
///
/// Adds a Newtonian attraction from particle_to to particle_from.
/// Uses F = Gm1m2 r_hat / r^2 with gravity_factor = G.
///
/// \param particle_from  Particle receiving the force
/// \param particle_to    Source particle
/// \param gravity_factor   Gravitational constant G [L^3 M^-1 T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_point_gravity_2d(
    struct bj_particle_2d*       BJ_RESTRICT particle_from,
    const struct bj_particle_2d* BJ_RESTRICT particle_to,
    const bj_real                     gravity_factor
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Apply softened point gravity to avoid singularities at small r.
///
/// Uses F proportional to r / (r^2 + epsilon^2)^(3/2).
///
/// \param particle_from  Particle receiving the force
/// \param particle_to    Source particle
/// \param gravity_factor   Gravitational constant G [L^3 M^-1 T^-2]
/// \param epsilon          Softening length epsilon >= 0 [L]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_point_gravity_softened_2d(
    struct bj_particle_2d*       BJ_RESTRICT particle_from,
    const struct bj_particle_2d* BJ_RESTRICT particle_to,
    const bj_real                     gravity_factor,
    const bj_real                     epsilon
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Apply quadratic + linear drag to a particle's accumulator.
///
/// Drag magnitude = k1 * |v| + k2 * |v|^2 along -v_hat.
///
/// \param particle Particle to affect
/// \param k1         Linear drag coefficient [M T^-1]
/// \param k2         Quadratic drag coefficient [M L^-1]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_drag_2d(
    struct bj_particle_2d* particle,
    bj_real k1,
    bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Return scalar drag coefficient for a velocity.
///
/// Computes c = k1 * |v| + k2 * |v|^2.
///
/// \param vel Velocity vector [L T^-1]
/// \param k1  Linear drag coefficient [M T^-1]
/// \param k2  Quadratic drag coefficient [M L^-1]
/// \return c >= 0 [M T^-1]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_real bj_compute_particle_drag_coefficient_2d(
    const struct bj_vec2 vel,
    const bj_real k1,
    const bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Compute drag force for a velocity.
///
/// result = -c * v_hat with c as above. Zero if vel is near zero.
///
/// \param vel    Velocity vector [L T^-1]
/// \param k1     Linear drag coefficient [M T^-1]
/// \param k2     Quadratic drag coefficient [M L^-1]
/// \return Output force [M L T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_vec2 bj_compute_particle_drag_force_2d(
    struct bj_vec2  vel,
    const    bj_real k1,
    const    bj_real k2
);

////////////////////////////////////////////////////////////////////////////////
/// Angular
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief 2D angular state and properties (scalar angle about Z).
///
/// Angle units are radians. Damping is unitless per-step decay on angular velocity.
/// Inertia is represented via inverse inertia; use 0 for infinite inertia.
///
/// \param value            Angle theta [rad]
/// \param velocity         Angular velocity omega [rad T^-1]
/// \param acceleration     Angular acceleration alpha [rad T^-2]
/// \param torque           Accumulated torque tau for next step [M L^2 T^-2]
/// \param damping          Angular velocity damping factor in [0, 1]
/// \param inverse_inertia  Inverse moment of inertia I^-1 [M^-1 L^-2]
////////////////////////////////////////////////////////////////////////////////
struct bj_angular_2d {
    bj_real value;
    bj_real velocity;
    bj_real acceleration;
    bj_real torque;
    bj_real damping;
    bj_real inverse_inertia;
};
struct bj_angular_2d;

////////////////////////////////////////////////////////////////////////////////
/// \brief Add torque to the angular accumulator.
///
/// \param angular Angular state to modify
/// \param torque  Torque to add tau [M L^2 T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_angular_torque_2d(
    struct bj_angular_2d* angular,
    bj_real torque
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Semi-implicit Euler step for angular motion.
///
/// Integrates alpha and omega, applies damping, and advances theta. Clears torque.
///
/// \param angular     Angular state to integrate
/// \param delta_time  Time step [T]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_step_angular_2d(
    struct bj_angular_2d* angular,
    double         delta_time
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Rigid body with translational and angular components.
///
/// Particle terms are in world space. Angle is about +Z.
///
/// \param particle  Linear state (position, velocity, forces)
/// \param angular   Angular state (theta, omega, tau)
////////////////////////////////////////////////////////////////////////////////
struct bj_rigid_body_2d {
    struct bj_particle_2d particle;
    struct bj_angular_2d  angular;
};
struct bj_rigid_body_2d;

////////////////////////////////////////////////////////////////////////////////
/// \brief Apply a world-space force at the center of mass.
///
/// Only affects linear state. Use external torque to affect rotation.
///
/// \param body   Rigid body to modify
/// \param force  Force to add [M L T^-2]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_apply_rigidbody_force_2d(
    struct bj_rigid_body_2d* body,
    const struct bj_vec2 force
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Step rigid body linear and angular states.
///
/// Calls the corresponding particle and angular integrators. Clears accumulators.
///
/// \param body        Rigid body to integrate
/// \param delta_time  Time step [T]
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_step_rigid_body_2d(
    struct bj_rigid_body_2d* body,
    double            delta_time
);

////////////////////////////////////////////////////////////////////////////////
/// \}
////////////////////////////////////////////////////////////////////////////////

#endif /* BJ_PHYSICS_2D_H */
