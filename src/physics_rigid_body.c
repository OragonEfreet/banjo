#include <banjo/physics_2d.h>
#include "check.h"

void bj_rigid_body_add_force_2d(
    bj_rigid_body_2d* body,
    const bj_vec2 force
) {
    bj_check(body);
    bj_point_mass_add_force_2d(&body->point_mass, force);
    // TODO Add torque by mean of force
}

void bj_integrate_rigid_body_2d(
    bj_angular_2d* angular,
    double         delta_time
) {
}


