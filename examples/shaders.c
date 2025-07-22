////////////////////////////////////////////////////////////////////////////////
/// \example shaders.c
/// A small example of using \ref bj_bitmap_apply_shader to generate animated
/// images.
//  This shader comes from https://www.shadertoy.com/view/mtyGWy
//  Designed by kishimisu at https://www.youtube.com/watch?v=f4s1h2YETNY
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/shader.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define CANVAS_W 512
#define CANVAS_H 512

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;

void palette(bj_vec3 res, float t) {
    const float f = 6.28318f;

    const bj_vec3 a = {.5f, .5f, .5f};
    const bj_vec3 b = {.5f, .5f, .5f};
    const bj_vec3 c = {1.f, 1.f, 1.f};
    const bj_vec3 d = {.263f, .416f, .557f};

    res[0] = a[0] + b[0] * bj_cosf(f * (c[0] * t + d[0]));
    res[1] = a[1] + b[1] * bj_cosf(f * (c[1] * t + d[1]));
    res[2] = a[2] + b[2] * bj_cosf(f * (c[2] * t + d[2]));
}

int shader_code(bj_vec3 frag_color, const bj_vec2 frag_coords, void* data) {
    float time = *(float*)data;

    bj_vec2 uv;
    bj_vec3 final_color = { 0 };

    bj_vec2_copy(uv, frag_coords);
    const float uv0_len = bj_vec2_len(uv);
    
    for(float i = 0.f ; i < 4.f ; ++i) {
        bj_vec3 col;
        palette(col, uv0_len + i * .4f + time * .4f);
        
        bj_vec2_scale(uv, uv, 1.5f);
        bj_vec2_apply(uv, uv, bj_fract);
        bj_vec2_sub(uv, uv, (bj_vec2) { .5f, .5f });

        const float d = bj_powf(0.01f / (
            bj_fabsf(
                bj_sinf(
                    bj_vec2_len(uv) * bj_expf(-uv0_len) * 8.f + time
                ) / 8.f
            )
         ), 1.2f);

        bj_vec3_scale(col, col, d);
        bj_vec3_add(final_color, final_color, col);
    }

    bj_vec3_copy(frag_color, final_color);
    return 1;
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Shader Art Coding Introduction", 1000, 500, CANVAS_W, CANVAS_H, 0);
    bj_set_key_callback(bj_close_on_escape);

    framebuffer = bj_window_get_framebuffer(window, 0);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    float time = (float)bj_get_time();
    bj_bitmap_apply_shader(framebuffer, shader_code, &time, BJ_SHADER_STANDARD_FLAGS);
    bj_window_update_framebuffer(window);
    bj_sleep(15);

    return bj_window_should_close(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_window_del(window);
    bj_end(0);
    return status;
}

