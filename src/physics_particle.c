#include <banjo/assert.h>
#include <banjo/math.h>
#include <banjo/physics.h>

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
    bj_assert(BJ_REAL_GT(dt, BJ_F(0.0)));

    damping = bj_clamp(damping, BJ_F(0.0), BJ_F(1.0));
    const bj_real damp_factor = bj_powf(damping, dt);

    for (size_t i = 0; i < len; ++i) {
        const bj_real a = accel[i] + forces[i] * inv_mass;
        bj_real v = vel[i] + a * dt;
        v *= damp_factor;
        vel[i] = v;
        pos[i] += v * dt;
    }
}

BANJO_EXPORT void bj_particle_integrate(
    bj_real* out_pos,
    bj_real* out_vel,
    bj_real  pos,
    bj_real  vel,
    bj_real  accel,
    bj_real  forces,
    bj_real  inv_mass,
    bj_real  damping,
    bj_real  dt
) {
    bj_assert(BJ_REAL_GT(dt, BJ_F(0.0)));
    damping = bj_clamp(damping, BJ_F(0.0), BJ_F(1.0));
    const bj_real damp_factor = bj_powf(damping, dt);

    const bj_real a = accel + forces * inv_mass;
    bj_real v = vel + a * dt;
    v *= damp_factor;

    *out_vel = v;
    *out_pos = pos + v * dt;
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


