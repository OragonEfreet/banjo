// pong_rt_shader_2d_raytracer.c
//
// Pong clone rendered by a simple 2D ray‐tracer: circles for the ball,
// rectangles for paddles, with smooth edges (anti‐aliasing).

#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/linmath.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/shader.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>
#include <banjo/math.h>    // bj_clamp, bj_smoothstep
#include <math.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Compile‐time, customize before building.

#define SCREEN_W       800
#define SCREEN_H       600
#define BALL_SIZE      16.0f
#define RACKET_MARGIN 100.0f
#define RACKET_LENGTH 120.0f
#define RACKET_WIDTH   24.0f
#define RACKET_VELOCITY 250.0f

////////////////////////////////////////////////////////////////////////////////
// Game data

static struct {
    struct { bj_vec2 pos, vel; } ball;
    struct { float y; bj_bool up, down; } racket[2];
    bj_bool running;
} game = {
    .ball    = {{SCREEN_W/2, SCREEN_H/2}, {200.0f,200.0f}},
    .racket  = {{SCREEN_H/2,BJ_FALSE,BJ_FALSE},{SCREEN_H/2,BJ_FALSE,BJ_FALSE}},
    .running = BJ_FALSE,
};

static bj_window*   window      = NULL;
static bj_bitmap*   framebuffer = NULL;
static bj_stopwatch stopwatch    = {0};

////////////////////////////////////////////////////////////////////////////////
// 2D raytracing shader

static float sdf_circle(float dx, float dy, float r) {
    return sqrtf(dx*dx + dy*dy) - r;
}

static float sdf_rect(float dx, float dy, float hw, float hh) {
    float ax = fabsf(dx) - hw;
    float ay = fabsf(dy) - hh;
    float ox = fmaxf(ax, 0.0f);
    float oy = fmaxf(ay, 0.0f);
    return sqrtf(ox*ox + oy*oy) + fminf(fmaxf(ax, ay), 0.0f);
}

int pong_rt_shader_2d(bj_vec3 out_color, const bj_vec2 pix, void* _ud) {
    (void)_ud;
    float x = pix[0], y = pix[1];

    // -- Ball SDF --
    float bx = game.ball.pos[0];
    float by = game.ball.pos[1];
    float d_ball = sdf_circle(x - bx, y - by, BALL_SIZE * 0.5f);

    // smooth edge for anti-aliasing
    float a_ball = 1.0f - bj_smoothstep(0.0f, 1.0f, d_ball);

    // -- Left Paddle SDF --
    float lx = RACKET_MARGIN + RACKET_WIDTH * 0.5f;
    float ly = game.racket[0].y;
    float d_lp = sdf_rect(x - lx, y - ly,
                          RACKET_WIDTH * 0.5f,
                          RACKET_LENGTH * 0.5f);
    float a_lp = 1.0f - bj_smoothstep(0.0f, 1.0f, d_lp);

    // -- Right Paddle SDF --
    float rx = SCREEN_W - RACKET_MARGIN - RACKET_WIDTH * 0.5f;
    float ry = game.racket[1].y;
    float d_rp = sdf_rect(x - rx, y - ry,
                          RACKET_WIDTH * 0.5f,
                          RACKET_LENGTH * 0.5f);
    float a_rp = 1.0f - bj_smoothstep(0.0f, 1.0f, d_rp);

    // -- Combine coverage --
    float cover = fmaxf(fmaxf(a_ball, a_lp), a_rp);

    // background black
    out_color[0] = out_color[1] = out_color[2] = 0.0f;
    // shapes white
    out_color[0] += cover;
    out_color[1] += cover;
    out_color[2] += cover;

    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Input & update (unchanged)

static void key_callback(bj_window* w, const bj_key_event* e) {
    (void)w;
    static const struct { bj_key up, down; } map[2] = {
        {BJ_KEY_D, BJ_KEY_F},
        {BJ_KEY_K, BJ_KEY_J},
    };
    for (int i = 0; i < 2; ++i) {
        game.racket[i].up   = (e->key == map[i].up   && e->action == BJ_PRESS);
        game.racket[i].down = (e->key == map[i].down && e->action == BJ_PRESS);
    }
    if (e->key == BJ_KEY_ESCAPE && e->action == BJ_RELEASE)
        bj_window_set_should_close(w);
    else
        game.running = BJ_TRUE;
}

static void update_game(double dt) {
    if (game.running) {
        float half = BALL_SIZE * 0.5f;
        bj_vec2 nxt; bj_vec2_add_scaled(nxt, game.ball.pos, game.ball.vel, dt);
        if (nxt[1] >= SCREEN_H - half || nxt[1] < half)
            game.ball.vel[1] = -game.ball.vel[1];
        if (nxt[0] >= SCREEN_W - half || nxt[0] < half)
            game.ball.vel[0] = -game.ball.vel[0];
        bj_vec2_add_scaled(game.ball.pos, game.ball.pos, game.ball.vel, dt);
    }
    for (int i = 0; i < 2; ++i) {
        float dir = (game.racket[i].down ? 1.0f : 0.0f)
                  - (game.racket[i].up   ? 1.0f : 0.0f);
        float ny = game.racket[i].y + dir * dt * RACKET_VELOCITY;
        float lo = RACKET_LENGTH * 0.5f, hi = SCREEN_H - lo;
        if (ny > lo && ny < hi)
            game.racket[i].y = ny;
    }
}

////////////////////////////////////////////////////////////////////////////////
// App callbacks

int bj_app_begin(void** ud, int argc, char* argv[]) {
    (void)ud; (void)argc; (void)argv;
    bj_error* err = NULL;
    if (!bj_begin(&err)) {
        bj_err("Init error 0x%X: %s", err->code, err->message);
        return bj_callback_exit_error;
    }
    window      = bj_window_new("Pong 2D Raytracer", 0, 0, SCREEN_W, SCREEN_H, 0);
    framebuffer = bj_window_get_framebuffer(window, 0);
    bj_set_key_callback(key_callback);
    return bj_callback_continue;
}

int bj_app_iterate(void* ud) {
    (void)ud;
    bj_dispatch_events();
    update_game(bj_stopwatch_step_delay(&stopwatch));

    // replace draw() with our shader
    bj_bitmap_apply_shader(
        framebuffer,
        pong_rt_shader_2d,
        NULL,
        0
    );

    bj_window_update_framebuffer(window);
    bj_sleep(15);
    return bj_window_should_close(window)
         ? bj_callback_exit_success
         : bj_callback_continue;
}

int bj_app_end(void* ud, int status) {
    (void)ud;
    bj_window_del(window);
    bj_end(0);
    return status;
}
