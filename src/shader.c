#include <banjo/shader.h>

#include "bitmap_t.h"
#include "check.h"

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
    float t = fminf(fmaxf((x - edge0) / (edge1 - edge0), 0.0f), 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

int bj_mod(
    float x,
    float y
) {
    return x - y * floorf(x / y);
}

float bj_fract(
    float x
) {
    return x - floorf(x);
}

void bj_bitmap_apply_shader(
    bj_bitmap* p_bitmap,
    bj_bitmap_shading_fn_t p_shader,
    void* p_data,
    uint8_t flags
) {
    bj_check(p_bitmap);
    bj_check(p_shader);

    const char invert_x = (flags & BJ_SHADER_INVERT_X) == BJ_SHADER_INVERT_X;
    const char invert_y = (flags & BJ_SHADER_INVERT_Y) == BJ_SHADER_INVERT_Y;
    const char clamp_color = (flags & BJ_SHADER_CLAMP_COLOR) == BJ_SHADER_CLAMP_COLOR;
    const char normalize_coords = (flags & BJ_SHADER_NORMALIZE_COORDS) == BJ_SHADER_NORMALIZE_COORDS;
    const char center_coords = (flags & BJ_SHADER_CENTER_COORDS) == BJ_SHADER_CENTER_COORDS;

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    const float width = (float)p_bitmap->width;
    const float height = (float)p_bitmap->height;

    const bj_vec2 center_shift = {
        normalize_coords ? .5f : (width / 2.f),
        normalize_coords ? .5f : (height / 2.f),
    };

    const bj_vec2 center_scale = {
        normalize_coords ? 2.f : (width * 2.f),
        normalize_coords ? 2.f : (height * 2.f),
    };

    


    for (size_t y = 0; y < p_bitmap->height; ++y) {
        for (size_t x = 0; x < p_bitmap->width; ++x) {

            bj_bitmap_rgb(p_bitmap, x, y, &r, &g, &b);

            bj_vec3 color = {
                (float)r / 255.0f,
                (float)g / 255.0f,
                (float)b / 255.0f,
            };

            bj_vec2 frag_coords = {
                (float)(invert_x ? width - x : x),
                (float)(invert_y ? height - y : y),
            };

            if (normalize_coords) {
                frag_coords[0] /= width;
                frag_coords[1] /= height;
            }

            if (center_coords) {
                bj_vec2_sub(frag_coords, frag_coords, center_shift);
                bj_vec2_scale_each(frag_coords, frag_coords, center_scale);
            }

            if (p_shader(color, frag_coords, p_data) > 0) {
                
                if (clamp_color) {
                    color[0] = bj_clamp(color[0], 0.f, 1.f);
                    color[1] = bj_clamp(color[1], 0.f, 1.f);
                    color[2] = bj_clamp(color[2], 0.f, 1.f);
                }

                r = (uint8_t)(color[0] * 255.0f);
                g = (uint8_t)(color[1] * 255.0f);
                b = (uint8_t)(color[2] * 255.0f);

                bj_bitmap_put_pixel(p_bitmap, x, y,bj_bitmap_pixel_value(p_bitmap, r, g, b));
            }
        }
    }
}

