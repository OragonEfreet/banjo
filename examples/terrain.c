////////////////////////////////////////////////////////////////////////////////
/// \example drawing_2d.c
///  2D primitive drawing demo.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/math.h>
#include <banjo/random.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define SCREEN_W 3000
#define SCREEN_H 800

#define N_EDGES (SCREEN_W / 2)

int t_x[N_EDGES] = {0};
int t_y[N_EDGES] = {0};

bj_window* window = 0;

static void init_terrain() {
    for(size_t i = 0 ; i < N_EDGES ; ++i) {
        t_x[i] = ((bj_real)i / N_EDGES) * SCREEN_W;
    }
}

bj_real mapv(bj_real v, bj_real ol, bj_real oh, bj_real nl, bj_real nh) {
    return nl + (v * ((nh - nl) / (oh - ol)));
}

static void terrain_naive() {
    for(size_t i = 0 ; i < N_EDGES ; ++i) {
        // Naive version
        const bj_real value = bj_uniform_real_distribution(bj_pcg32_generator, 0, BJ_FZERO, BJ_F(1.0));
        const int y = mapv(value, BJ_FZERO, BJ_F(1.0), BJ_FZERO, BJ_F(100.0));

        t_y[i] = SCREEN_H / 2 - y;

    }
}

static bj_real linp(bj_real a, bj_real b, bj_real mu) {
    return a * (BJ_F(1.0) - mu) + b * mu;
}

static bj_real cosp(bj_real a, bj_real b, bj_real mu) {
    const bj_real mu2 = (BJ_F(1.0) - bj_cos(mu * BJ_PI)) / BJ_F(2.0);
    return a * (BJ_F(1.0) - mu2) + b * mu2;
}


static void prepare_terrain() {
    terrain_naive();
}

static void draw(bj_bitmap* bmp) {
    bj_clear_bitmap(bmp);
    const uint32_t color = bj_make_bitmap_pixel(bmp, 0xFF, 0xFF, 0xFF);
    bj_draw_polyline(bmp, N_EDGES, t_x, t_y, BJ_FALSE, color);
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_initialize(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_bind_window("Simple Text", 100, 100, SCREEN_W, SCREEN_H, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    init_terrain();
    prepare_terrain();

    bj_bitmap* framebuffer = bj_get_window_framebuffer(window, 0);
    draw(framebuffer);
    bj_update_window_framebuffer(window);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_sleep(300);

    return bj_should_close_window(window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_unbind_window(window);
    bj_shutdown(0);
    return status;
}

