#include <banjo/physics_2d.h>
#include <check.h>

void bj_apply_angular_torque_2d(
    struct bj_angular_2d* angular,
    bj_real torque
) {
    bj_check(angular);
    angular->torque += torque;
}

void bj_step_angular_2d(struct bj_angular_2d* a, double dt) {
    bj_check(a);
    if (a->inverse_inertia != BJ_FZERO) {
        a->velocity = (
            a->velocity + (a->acceleration + a->torque * a->inverse_inertia) * (bj_real)dt
        ) * bj_pow(a->damping, (bj_real)dt);
        a->value += a->velocity * (bj_real)dt;

        if (a->value > BJ_PI) {
            a->value -= 2.0 * BJ_PI;
        } else if (a->value < -BJ_PI) {
            a->value += 2.0 * BJ_PI;
        }
    } else {
        a->velocity = BJ_FZERO;
        a->acceleration = BJ_FZERO;
    }
    a->torque = BJ_FZERO;
}

