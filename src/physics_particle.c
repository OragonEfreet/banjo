#include <banjo/assert.h>
#include <banjo/math.h>
#include <banjo/physics.h>
#include <banjo/physics_2d.h>
#include <banjo/physics_3d.h>
#include <banjo/vec.h>

#include "check.h"

static BJ_INLINE void bj_particle_integrate_array(
    size_t                     len,
    bj_real*       BJ_RESTRICT pos,
    bj_real*       BJ_RESTRICT vel,
    const bj_real* BJ_RESTRICT accel,
    const bj_real* BJ_RESTRICT forces,
    bj_real                    inv_mass,
    bj_real                    damping,
    bj_real                    dt
) {
    if (inv_mass == BJ_FZERO) {
        return;
    }

    const bj_real damp_factor = bj_pow(damping, dt);

    for (size_t i = 0; i < len; ++i) {
        const bj_real acc = accel[i] + forces[i] * inv_mass;
        pos[i] += vel[i] * dt;
        vel[i] = (vel[i] + acc * dt) * damp_factor;
    }
}

void bj_integrate_particle_2d(
    bj_particle_2d* p_particle,
    bj_real dt
) {
    bj_check(p_particle);

    bj_particle_integrate_array(2,
        p_particle->position,
        p_particle->velocity,
        p_particle->acceleration,
        p_particle->forces,
        p_particle->inverse_mass,
        p_particle->damping,
        dt
    );
    bj_vec2_zero(p_particle->forces);
}

void bj_integrate_particle_3d(
    bj_particle_3d* p_particle,
    bj_real dt
) {
    bj_check(p_particle);

    bj_particle_integrate_array(3,
        p_particle->position,
        p_particle->velocity,
        p_particle->acceleration,
        p_particle->forces,
        p_particle->inverse_mass,
        p_particle->damping,
        dt
    );
    bj_vec2_zero(p_particle->forces);
}

void bj_accumulate_world_gravity_2d(
    bj_particle_2d* p_particle,
    bj_real         gravity
) {
    p_particle->forces[1] -= gravity;
}

void bj_accumulate_world_gravity_3d(
    bj_particle_3d* p_particle,
    bj_real         gravity
) {
    p_particle->forces[1] -= gravity;
}

void bj_accumulate_drag_2d(
    bj_particle_2d* p_particle,
    bj_real k1,
    bj_real k2
) {
    bj_vec2 force;
    bj_particle_drag_force_2d(force, p_particle->velocity, k1, k2);
    bj_vec2_add(p_particle->forces, p_particle->forces, force);
}

void bj_accumulate_drag_3d(
    bj_particle_3d* p_particle,
    bj_real k1,
    bj_real k2
) {
    bj_vec3 force;
    bj_particle_drag_force_3d(force, p_particle->velocity, k1, k2);
    bj_vec3_add(p_particle->forces, p_particle->forces, force);
}

bj_real bj_particle_drag_coefficient_3d(
    const bj_vec3 vel,
    const bj_real k1,
    const bj_real k2
) {
    bj_real coef = bj_vec3_len(vel);
    if(bj_real_is_zero(coef)) {
        return BJ_FZERO;
    }
    return k1 * coef + k2 * coef * coef;
}

bj_real bj_particle_drag_coefficient_2d(
    const bj_vec2 vel,
    const bj_real k1,
    const bj_real k2
) {
    bj_real coef = bj_vec2_len(vel);
    if(bj_real_is_zero(coef)) {
        return BJ_FZERO;
    }
    return k1 * coef + k2 * coef * coef;
}

bj_bool bj_particle_drag_force_3d(
    bj_real result[BJ_RESTRICT static 3],
    const bj_real vel[BJ_RESTRICT static 3],
    const bj_real k1,
    const bj_real k2
) {
    bj_real coef = bj_particle_drag_coefficient_3d(vel, k1, k2);

    if(bj_real_is_zero(coef)) {
        bj_vec3_set(result, BJ_FZERO, BJ_FZERO, BJ_FZERO);
        return BJ_FALSE;
    }
    bj_vec3_set_len(result, vel, -coef);
    return BJ_TRUE;
}

bj_bool bj_particle_drag_force_2d(
    bj_real result[BJ_RESTRICT static 2],
    const bj_real vel[BJ_RESTRICT static 2],
    const bj_real k1,
    const bj_real k2
) {
    bj_real coef = bj_particle_drag_coefficient_2d(vel, k1, k2);

    if(bj_real_is_zero(coef)) {
        bj_vec2_set(result, BJ_FZERO, BJ_FZERO);
        return BJ_FALSE;
    }
    bj_vec2_set_len(result, vel, -coef);
    return BJ_TRUE;
}


