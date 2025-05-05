#pragma once
#include <banjo/api.h>
#include <banjo/bitmap.h>
#include <banjo/math.h>

typedef int (*bj_bitmap_shading_fn_t)(const bj_vec2, bj_vec3, void*);

BANJO_EXPORT float bj_smoothstep(
    float edge0,
    float edge1,
    float x
);

BANJO_EXPORT void bj_bitmap_apply_shader(
    bj_bitmap* p_bitmap,
    bj_bitmap_shading_fn_t p_shader,
    void* p_data
);


