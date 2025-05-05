////////////////////////////////////////////////////////////////////////////////
/// \example window.c
/// How to open and close windows.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/system.h>
#include <banjo/window.h>
#include <banjo/shader.h>

#define CANVAS_W 512
#define CANVAS_H 512

// This shader comes from https://www.shadertoy.com/view/mtyGWy
// Designed by kishimisu at https://www.youtube.com/watch?v=f4s1h2YETNY
void palette(bj_vec3 res, float t, const bj_vec3 a, const bj_vec3 b, const bj_vec3 c, const bj_vec3 d) {
    const float f = 6.28318f;
    res[0] = a[0] + b[0] * cosf(f * (c[0] * t + d[0]));
    res[1] = a[1] + b[1] * cosf(f * (c[1] * t + d[1]));
    res[2] = a[2] + b[2] * cosf(f * (c[2] * t + d[2]));
}

void my_palette(bj_vec3 res, float t) {
    palette(res, t,
        (bj_vec3){.5f, .5f, .5f },
        (bj_vec3){.5f, .5f, .5f },
        (bj_vec3){1.f, 1.f, 1.f },
        (bj_vec3){.263f, .416f, .557f }
     );
}

int shader_code(bj_vec3 frag_color, const bj_vec2 frag_coords, void* data) {
    float time = *(float*)data;

    bj_vec2 uv;
    bj_vec2_copy(uv, frag_coords);
    bj_vec2 uv0;
    bj_vec2_copy(uv0, frag_coords);
    bj_vec3 final_color = { 0 };

    for(float i = 0.f ; i < 4.f ; ++i) {
        
        bj_vec2_scale(uv, uv, 1.5f);
        bj_vec2_apply(uv, uv, bj_fract);
        bj_vec2_sub(uv, uv, (bj_vec2) { .5f, .5f });

        float d = bj_vec2_len(uv) * expf(-bj_vec2_len(uv0));

        bj_vec3 col;
        my_palette(col, bj_vec2_len(uv0) + i * .4f + time * .4f);

        d = sinf(d * 8.f + time) / 8.f;
        d = fabsf(d);
        
        d = powf(0.01f / d, 1.2f);

        bj_vec3_scale(col, col, d);
        bj_vec3_add(final_color, final_color, col);
    }

    bj_vec3_copy(frag_color, final_color);
    
    return 1;
}


int main(void) {

   if (bj_system_init(0)) {    

        bj_window* window = bj_window_new("Shader Art Coding Introduction", 1000, 500, CANVAS_W, CANVAS_H, 0);
        bj_window_set_key_event(window, bj_close_on_escape);

        bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);

        bj_bitmap_shading_fn_t current_shader = shader_code;
        
        while (!bj_window_should_close(window)) {
            bj_poll_events();

            float time = (float)bj_get_time();

            bj_bitmap_apply_shader(framebuffer, current_shader, &time, BJ_SHADER_STANDARD_FLAGS);

            bj_window_update_framebuffer(window);
            bj_sleep(15);
        }

        bj_window_del(window);

        bj_system_dispose(0);
    }

    return 0;
}
