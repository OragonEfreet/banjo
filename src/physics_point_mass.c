#include <banjo/assert.h>
#include <banjo/math.h>
#include <banjo/physics.h>
#include <banjo/physics_2d.h>
#include <banjo/vec.h>

#include "check.h"

void bj_point_mass_set_mass_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real         mass
) {
    bj_check(p_point_mass);
    bj_assert(!bj_real_is_zero(mass));
    p_point_mass->inverse_mass = BJ_F(1.0) / mass;
}

void bj_point_mass_add_force_2d(
    bj_point_mass_2d* p_point_mass,
    const bj_vec2 force
) {
    bj_check(p_point_mass);
    bj_vec2_add(p_point_mass->forces, p_point_mass->forces, force);
}

void bj_integrate_point_mass_2d(bj_point_mass_2d* p, bj_real dt) {
    bj_check(p);
    if (p->inverse_mass != BJ_FZERO) {
        bj_vec2 acc;
        bj_vec2_add_scaled(acc, p->acceleration, p->forces, p->inverse_mass);
        bj_vec2_add_scaled(p->velocity, p->velocity, acc, dt);
        bj_vec2_scale(p->velocity, p->velocity, bj_pow(p->damping, dt));
        bj_vec2_add_scaled(p->position, p->position, p->velocity, dt);
    }
    bj_vec2_zero(p->forces);
}

void bj_accumulate_world_gravity_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real         gravity
) {
    p_point_mass->forces[1] -= gravity;
}

void bj_accumulate_point_gravity_2d(
    bj_point_mass_2d* BJ_RESTRICT       p_point_mass_from,
    const bj_point_mass_2d* BJ_RESTRICT p_point_mass_to,
    const bj_real                     gravity_factor
) {
    bj_vec2 force = {BJ_FZERO, BJ_FZERO};

    bj_real g = bj_newton_gravitation(
        BJ_F(1.0) / p_point_mass_from->inverse_mass,
        BJ_F(1.0) / p_point_mass_to->inverse_mass,
        bj_vec2_dist(p_point_mass_from->position, p_point_mass_to->position),
        gravity_factor
    );

    bj_vec2_sub(force, p_point_mass_to->position, p_point_mass_from->position);
    bj_vec2_set_len(force, force, g);

    bj_point_mass_add_force_2d(p_point_mass_from, force);
}

void bj_accumulate_point_gravity_softened_2d(
    bj_point_mass_2d*       BJ_RESTRICT p_point_mass_from,
    const bj_point_mass_2d* BJ_RESTRICT p_point_mass_to,
    const bj_real                     gravity_factor,
    const bj_real                     epsilon
) {
    bj_vec2 force = { BJ_FZERO, BJ_FZERO };

    const bj_real m1 = BJ_F(1.0) / p_point_mass_from->inverse_mass;
    const bj_real m2 = BJ_F(1.0) / p_point_mass_to->inverse_mass;
    const bj_real r  = bj_vec2_dist(p_point_mass_from->position, p_point_mass_to->position);

    const bj_real g = bj_newton_plummer_gravitation(m1, m2, r, gravity_factor, epsilon);

    bj_vec2_sub(force, p_point_mass_to->position, p_point_mass_from->position);
    bj_vec2_set_len(force, force, g);

    bj_point_mass_add_force_2d(p_point_mass_from, force);
}


void bj_accumulate_drag_2d(
    bj_point_mass_2d* p_point_mass,
    bj_real k1,
    bj_real k2
) {
    bj_vec2 force;
    bj_point_mass_drag_force_2d(force, p_point_mass->velocity, k1, k2);
    bj_point_mass_add_force_2d(p_point_mass, force);
}

bj_real bj_point_mass_drag_coefficient_2d(
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

bj_bool bj_point_mass_drag_force_2d(
    bj_real result[BJ_RESTRICT static 2],
    const bj_real vel[BJ_RESTRICT static 2],
    const bj_real k1,
    const bj_real k2
) {
    bj_real coef = bj_point_mass_drag_coefficient_2d(vel, k1, k2);

    if(bj_real_is_zero(coef)) {
        bj_vec2_set(result, BJ_FZERO, BJ_FZERO);
        return BJ_FALSE;
    }
    bj_vec2_set_len(result, vel, -coef);
    return BJ_TRUE;
}


