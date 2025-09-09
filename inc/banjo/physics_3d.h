#ifndef BJ_PHYSICS_3D_H
#define BJ_PHYSICS_3D_H

#include <banjo/api.h>
#include <banjo/math.h>

struct bj_particle_3d_t {
    bj_vec3 position;
    bj_vec3 velocity;
    bj_vec3 acceleration;
    bj_vec3 forces;
    bj_real damping;
    bj_real inverse_mass;
};

typedef struct bj_particle_3d_t bj_particle_3d;

BANJO_EXPORT bj_real bj_particle_drag_coefficient_3d(
    const bj_vec3 vel,
    const bj_real k1,
    const bj_real k2
);

BANJO_EXPORT bj_bool bj_particle_drag_force_3d(
    bj_real result[BJ_RESTRICT static 3],
    const bj_real vel[BJ_RESTRICT static 3],
    const bj_real k1,
    const bj_real k2
);

BANJO_EXPORT void bj_accumulate_world_gravity_3d(
    bj_particle_3d* p_particle,
    bj_real         gravity
);

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

#endif
