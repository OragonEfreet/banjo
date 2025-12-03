#include <banjo/physics_2d.h>
#include <check.h>

void bj_apply_rigidbody_force_2d(
    bj_rigid_body_2d* body,
    const bj_vec2 force
) {
    bj_check(body);
    bj_apply_particle_force_2d(&body->particle, force);
    // TODO Add torque by mean of force
}

void bj_step_rigid_body_2d(
    bj_rigid_body_2d* body,
    double            delta_time
) {
    bj_check(body);
    bj_step_particle_2d(&body->particle, delta_time);
    bj_step_angular_2d(&body->angular, delta_time);
}


