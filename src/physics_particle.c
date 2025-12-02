#include <banjo/assert.h>
#include <banjo/math.h>
#include <banjo/physics.h>
#include <banjo/physics_2d.h>
#include <banjo/vec.h>

#include <check.h>

void bj_set_particle_mass_2d(
    bj_particle_2d* p_particle,
    bj_real         mass
) {
    bj_check(p_particle);
    bj_assert(!bj_real_is_zero(mass));
    p_particle->inverse_mass = BJ_F(1.0) / mass;
}

void bj_apply_particle_force_2d(
    bj_particle_2d* p_particle,
    bj_vec2 force
) {
    bj_check(p_particle);
    p_particle->forces = bj_vec2_add(p_particle->forces, force);
}

void bj_step_particle_2d(bj_particle_2d* p, bj_real dt) {
    bj_check(p);
    if (p->inverse_mass != BJ_FZERO) {
        bj_vec2 acc = bj_vec2_add_scaled(p->acceleration, p->forces, p->inverse_mass);
        p->velocity = bj_vec2_add_scaled(p->velocity, acc, dt);
        p->velocity = bj_vec2_scale(p->velocity, bj_pow(p->damping, dt));
        p->position = bj_vec2_add_scaled(p->position, p->velocity, dt);
    }
    p->forces = BJ_VEC2_ZERO;
}

void bj_apply_gravity_2d(
    bj_particle_2d* p_particle,
    bj_real         gravity
) {
    p_particle->forces.y -= gravity;
}

void bj_apply_point_gravity_2d(
    bj_particle_2d* BJ_RESTRICT       p_particle_from,
    const bj_particle_2d* BJ_RESTRICT p_particle_to,
    const bj_real                     gravity_factor
) {
    bj_real g = bj_newton_gravitation(
        BJ_F(1.0) / p_particle_from->inverse_mass,
        BJ_F(1.0) / p_particle_to->inverse_mass,
        bj_vec2_distance(p_particle_from->position, p_particle_to->position),
        gravity_factor
    );

    const bj_vec2 force = bj_vec2_scale_to_len(bj_vec2_sub(
        p_particle_to->position,
        p_particle_from->position
    ), g);

    bj_apply_particle_force_2d(p_particle_from, force);
}

void bj_apply_point_gravity_softened_2d(
    bj_particle_2d*       BJ_RESTRICT p_particle_from,
    const bj_particle_2d* BJ_RESTRICT p_particle_to,
    const bj_real                     gravity_factor,
    const bj_real                     epsilon
) {
    const bj_real m1 = BJ_F(1.0) / p_particle_from->inverse_mass;
    const bj_real m2 = BJ_F(1.0) / p_particle_to->inverse_mass;
    const bj_real r  = bj_vec2_distance(p_particle_from->position, p_particle_to->position);

    const bj_real g = bj_newton_plummer_gravitation(m1, m2, r, gravity_factor, epsilon);

    const bj_vec2 force = bj_vec2_scale_to_len(bj_vec2_sub(
        p_particle_to->position,
        p_particle_from->position
    ), g);

    bj_apply_particle_force_2d(p_particle_from, force);
}


void bj_apply_drag_2d(
    bj_particle_2d* p_particle,
    bj_real k1,
    bj_real k2
) {
    bj_apply_particle_force_2d(
        p_particle,
        bj_compute_particle_drag_force_2d(p_particle->velocity, k1, k2)
    );
}

bj_real bj_compute_particle_drag_coefficient_2d(
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

bj_vec2 bj_compute_particle_drag_force_2d(
    bj_vec2  vel,
    const    bj_real k1,
    const    bj_real k2
) {
    const bj_real coef = bj_compute_particle_drag_coefficient_2d(vel, k1, k2);
    return bj_real_is_zero(coef) ? BJ_VEC2_ZERO : bj_vec2_scale_to_len(vel, -coef);
}


