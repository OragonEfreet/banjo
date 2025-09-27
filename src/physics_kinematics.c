#include <banjo/physics.h>

static BJ_INLINE void bj_kinematics_array(
    size_t                     n,
    bj_real*       BJ_RESTRICT out,
    const bj_real* BJ_RESTRICT position,
    const bj_real* BJ_RESTRICT velocity,
    const bj_real* BJ_RESTRICT acceleration,
    bj_real        time
) {
    for(size_t i = 0 ; i < n ; ++i) {
        out[i] = bj_galileo_position(position[i], velocity[i], acceleration[i], time);
    }
}

static BJ_INLINE void bj_kinematics_velocity_array(
    size_t                     n,
    bj_real*       BJ_RESTRICT out,
    const bj_real* BJ_RESTRICT velocity,
    const bj_real* BJ_RESTRICT acceleration,
    bj_real                    time
) {
    for (size_t i = 0; i < n; ++i) {
        out[i] = bj_galileo_velocity(velocity[i], acceleration[i], time);
    }
}

void bj_compute_kinematics_2d(
    BJ_ARRAY(bj_real, 2, out),
    BJ_CONST_ARRAY(bj_real, 2, position),
    BJ_CONST_ARRAY(bj_real, 2, velocity),
    BJ_CONST_ARRAY(bj_real, 2, acceleration),
    bj_real time
) {
    bj_kinematics_array(2, out, position, velocity, acceleration, time);
}

void bj_compute_kinematics_velocity_2d(
    BJ_ARRAY(bj_real, 2, out),
    BJ_CONST_ARRAY(bj_real, 2, velocity),
    BJ_CONST_ARRAY(bj_real, 2, acceleration),
    bj_real time
) {
    bj_kinematics_velocity_array(2, out, velocity, acceleration, time);
}

