#include <banjo/shader.h>

#include "bitmap_t.h"
#include "check.h"

float bj_smoothstep(
    float edge0,
    float edge1,
    float x
) {
    float t = fminf(fmaxf((x - edge0) / (edge1 - edge0), 0.0f), 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

void bj_bitmap_apply_shader(
    bj_bitmap* p_bitmap,
    bj_bitmap_shading_fn_t p_shader,
    void* p_data
) {
    bj_check(p_bitmap);
    bj_check(p_shader);

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    for (size_t y = 0; y < p_bitmap->width; ++y) {
        for (size_t x = 0; x < p_bitmap->height; ++x) {

            bj_bitmap_rgb(p_bitmap, x, y, &r, &g, &b);

            bj_vec3 color = {
                (float)r / 255.0f,
                (float)g / 255.0f,
                (float)b / 255.0f,
            };

            if (p_shader((bj_vec2) { (float)x, (float)y }, color, p_data) > 0) {
                bj_bitmap_put_pixel(
                    p_bitmap, x, y,
                    bj_bitmap_pixel_value(
                        p_bitmap,
                        (uint8_t)(color[0] * 255.0f),
                        (uint8_t)(color[1] * 255.0f),
                        (uint8_t)(color[2] * 255.0f)
                    )
                );
            }
        }
    }
}

