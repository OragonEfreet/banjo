#include <banjo/physics.h>
#include <banjo/physics_2d.h>

struct bj_vec2 bj_compute_kinematics_2d(
    struct bj_vec2  position,
    struct bj_vec2  velocity,
    struct bj_vec2  acceleration,
    bj_real  time
) {
    return (struct bj_vec2){
        .x = bj_galileo_position(position.x, velocity.x, acceleration.x, time),
        .y = bj_galileo_position(position.y, velocity.y, acceleration.y, time),
    };
}

struct bj_vec2 bj_compute_kinematics_velocity_2d(
    struct bj_vec2 velocity,
    struct bj_vec2 acceleration,
    bj_real time
) {
    return (struct bj_vec2){
        .x = bj_galileo_velocity(velocity.x, acceleration.x, time),
        .y = bj_galileo_velocity(velocity.y, acceleration.y, time),
    };
}

