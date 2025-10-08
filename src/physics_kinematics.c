#include <banjo/physics.h>

bj_vec2 bj_compute_kinematics_2d(
    bj_vec2  position,
    bj_vec2  velocity,
    bj_vec2  acceleration,
    bj_real  time
) {
    return (bj_vec2){
        .x = bj_galileo_position(position.x, velocity.x, acceleration.x, time),
        .y = bj_galileo_position(position.y, velocity.y, acceleration.y, time),
    };
}

bj_vec2 bj_compute_kinematics_velocity_2d(
    bj_vec2 velocity,
    bj_vec2 acceleration,
    bj_real time
) {
    return (bj_vec2){
        .x = bj_galileo_velocity(velocity.x, acceleration.x, time),
        .y = bj_galileo_velocity(velocity.y, acceleration.y, time),
    };
}

