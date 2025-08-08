#include <banjo/math.h>

float bj_clamp(
    float x,
    float min,
    float max
) {
    return (x < min) ? min : (x > max) ? max : x;
}

float bj_step(
    float edge,
    float x
) {
    return x < edge ? 0.0f : 1.0f;
}

float bj_smoothstep(
    float edge0,
    float edge1,
    float x
) {
    float t = bj_fminf(bj_fmaxf((x - edge0) / (edge1 - edge0), 0.0f), 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

int bj_mod(
    float x,
    float y
) {
    return x - y * bj_floorf(x / y);
}

float bj_fract(
    float x
) {
    return x - bj_floorf(x);
}

