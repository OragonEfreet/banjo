////////////////////////////////////////////////////////////////////////////////
/// \example shaders.c
/// A small example of using \ref bj_shader_bitmap to generate animated
/// images.
//  This shader comes from https://www.shadertoy.com/view/mtyGWy
//  Designed by kishimisu at https://www.youtube.com/watch?v=f4s1h2YETNY
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/renderer.h>
#include <banjo/shader.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define CANVAS_W 512
#define CANVAS_H 512

bj_window* window      = 0;
bj_renderer* renderer  = 0;
bj_bitmap* framebuffer = 0;

static bj_vec3 palette(bj_real t) {
    const bj_real f = BJ_F(6.28318);

    const bj_vec3 a = { BJ_F(0.5), BJ_F(0.5), BJ_F(0.5) };
    const bj_vec3 b = { BJ_F(0.5), BJ_F(0.5), BJ_F(0.5) };
    const bj_vec3 c = { BJ_F(1.0), BJ_F(1.0), BJ_F(1.0) };
    const bj_vec3 d = { BJ_F(0.263), BJ_F(0.416), BJ_F(0.557) };

    return (bj_vec3) {
        .x = a.x + b.x * bj_cos(f * (c.x * t + d.x)),
        .y = a.y + b.y * bj_cos(f * (c.y * t + d.y)),
        .z = a.z + b.z * bj_cos(f * (c.z * t + d.z)),
    };

  }

int shader_code(bj_vec3* frag_color, const bj_vec2 frag_coords, void* data) {
    bj_real time = *(bj_real*)data;

    bj_vec3 final_color = { BJ_FZERO, BJ_FZERO, BJ_FZERO };

    bj_vec2 uv = frag_coords;
    const bj_real uv0_len = bj_vec2_len(uv);
    
    for (bj_real i = BJ_FZERO; i < BJ_F(4.0); i += BJ_F(1.0)) {
        bj_vec3 col = palette(uv0_len + i * BJ_F(0.4) + time * BJ_F(0.4));
        
        uv = bj_vec2_scale(uv, BJ_F(1.5));
        uv = bj_vec2_map(uv, bj_fract);
        uv = bj_vec2_sub(uv, (bj_vec2){ BJ_F(0.5), BJ_F(0.5) });

        const bj_real d = bj_pow(
            BJ_F(0.01) / (
                bj_abs(
                    bj_sin(
                        bj_vec2_len(uv) * bj_exp(-uv0_len) * BJ_F(8.0) + time
                    ) / BJ_F(8.0)
                )
            ),
            BJ_F(1.2)
        );

        col = bj_vec3_scale(col, d);
        final_color = bj_vec3_add(final_color, col);
    }

    *frag_color = final_color;
    return 1;
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if (!bj_initialize(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("Shader Art Coding Introduction", 1000, 500, CANVAS_W, CANVAS_H, 0);

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    framebuffer = bj_get_framebuffer(renderer);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_real time = (bj_real)bj_run_time();
    bj_shader_bitmap(framebuffer, shader_code, &time, BJ_SHADER_STANDARD_FLAGS);
    bj_present(renderer, window);
    bj_sleep(15);

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_shutdown(0);
    return status;
}
