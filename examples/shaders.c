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

#define CANVAS_W 500
#define CANVAS_H 500

typedef struct uniform_t {
    struct { float w; float h; } resolution;
    struct { float x; float y; } mouse;
    float time;
} uniform;
static uniform s_uniform;

float plot(const bj_vec2 st, float pct) {
    return bj_smoothstep(pct - 0.02f, pct, st[1]) - bj_smoothstep(pct, pct + 0.02f, st[1]);
}

int shader_code(const bj_vec2 frag_coords, bj_vec3 frag_color, const uniform* u) {

    bj_vec2 st = {frag_coords[0] / u->resolution.w,(u->resolution.h - frag_coords[1]) / u->resolution.h};
    
    float y = bj_smoothstep(0.1f, 0.9f, st[0]);

    float pct = plot(st, y);

    bj_vec3_set(frag_color,
        (1.0f - pct) * y,
        (1.0f - pct) * y + pct,
        (1.0f - pct) * y
    );
    
    return 1;
}

void cursor_event(bj_window* p_window, int x, int y) {
    (void)p_window;
    s_uniform.mouse.x = (float)x;
    s_uniform.mouse.y = (float)y;
}

int main(void) {

   if (bj_system_init(0)) {    

        bj_window* window = bj_window_new("The Book of Shaders", 1000, 500, CANVAS_W, CANVAS_H, 0);
        bj_window_set_key_event(window, bj_close_on_escape);
        bj_window_set_cursor_event(window, cursor_event);

        bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);

        s_uniform.resolution.w = (float)CANVAS_W;
        s_uniform.resolution.h = (float)CANVAS_H;

        bj_bitmap_shading_fn_t current_shader = shader_code;
        
        while (!bj_window_should_close(window)) {
            bj_poll_events();

            s_uniform.time = (float)bj_get_time();

            bj_bitmap_apply_shader(framebuffer, current_shader, &s_uniform);

            bj_window_update_framebuffer(window);
            bj_sleep(30);
        }

        bj_window_del(window);

        bj_system_dispose(0);
    }

    return 0;
}
