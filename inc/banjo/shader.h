#pragma once
#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/math.h>

typedef int (*bj_bitmap_shading_fn_t)(bj_vec3, const bj_vec2, void*);

BANJO_EXPORT float bj_clamp(
    float x,
    float min,
    float max
);

BANJO_EXPORT float bj_step(
    float edge,
    float x
);

BANJO_EXPORT float bj_smoothstep(
    float edge0,
    float edge1,
    float x
);

BANJO_EXPORT float bj_fract(
    float x
);


#define BJ_SHADER_INVERT_X 0x01
#define BJ_SHADER_INVERT_Y 0x02
#define BJ_SHADER_CLAMP_COLOR 0x04
#define BJ_SHADER_NORMALIZE_COORDS 0x08
#define BJ_SHADER_CENTER_COORDS 0x10

#define BJ_SHADER_STANDARD_FLAGS (BJ_SHADER_INVERT_Y | BJ_SHADER_CLAMP_COLOR | BJ_SHADER_NORMALIZE_COORDS | BJ_SHADER_CENTER_COORDS)

BANJO_EXPORT void bj_bitmap_apply_shader(
    bj_bitmap* p_bitmap,
    bj_bitmap_shading_fn_t p_shader,
    void* p_data,
    uint8_t flags
);


