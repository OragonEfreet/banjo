#include <banjo/assert.h>
#include <banjo/math.h>
#include <banjo/physics.h>
#include <banjo/vec.h>

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

void bj_particle_integrate_2d(
    bj_real        pos[BJ_RESTRICT static 2],
    bj_real        vel[BJ_RESTRICT static 2],
    const bj_real  accel[BJ_RESTRICT static 2],
    const bj_real  forces[BJ_RESTRICT static 2],
    bj_real        inv_mass,
    bj_real        damping,
    bj_real        dt
) {
    bj_particle_integrate_array(2,
        pos,
        vel,
        accel,
        forces,
        inv_mass,
        damping,
        dt
    );
}

void bj_particle_integrate_3d(
    bj_real        pos[BJ_RESTRICT static 3],
    bj_real        vel[BJ_RESTRICT static 3],
    const bj_real  accel[BJ_RESTRICT static 3],
    const bj_real  forces[BJ_RESTRICT static 3],
    bj_real        inv_mass,
    bj_real        damping,
    bj_real        dt
) {
    bj_particle_integrate_array(3,
        pos,
        vel,
        accel,
        forces,
        inv_mass,
        damping,
        dt
    );
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


