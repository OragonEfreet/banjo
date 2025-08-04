// pong_rt_shader_crt_80s_glow.c
//
// Pong clone with animated 80’s neon grid, rounded paddles, ball shading,
// CRT distortion, scanlines, glow effects, and brightness boost.

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
#include <banjo/math.h>    // bj_clamp
#include <math.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Screen & game constants
#define SCREEN_W         800
#define SCREEN_H         600
#define BALL_SIZE        16
#define RACKET_MARGIN    100
#define RACKET_LENGTH    120
#define RACKET_WIDTH     24
#define RACKET_VELOCITY  (250.f)
#define PADDLE_CORNER_R  6.0f

// CRT & effect constants
#define CRT_K             0.08f    // barrel distortion
#define SCAN_ALPHA        0.7f     // scanline darkness
#define QUANT_LEVELS      4.0f     // color quantization
#define BRIGHTNESS        1.5f     // overall brightness
#define GRID_SCROLL_SPEED 0.5f     // slower scroll speed

// Glow constants (in px)
#define GLOW_RANGE_BALL    (BALL_SIZE * 0.75f)
#define GLOW_RANGE_PADDLE  12.0f
#define GLOW_INTENSITY     0.6f

////////////////////////////////////////////////////////////////////////////////
// Game state
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
static float       time_secs    = 0.0f;

////////////////////////////////////////////////////////////////////////////////
// fract helper
static float fract_f(float x) { return x - floorf(x); }

////////////////////////////////////////////////////////////////////////////////
// Shader: animated 80s CRT + glow
int pong_rt_shader_crt_80s_glow(bj_vec3 out_color, const bj_vec2 pix, void* _ud) {
    // _ud -> &time_secs
    float t = *(float*)_ud;

    // 1) Barrel distortion
    float x = pix[0], y = pix[1];
    float nx = (2.0f * x / (SCREEN_W - 1)) - 1.0f;
    float ny = 1.0f - (2.0f * y / (SCREEN_H - 1));
    float r2 = nx*nx + ny*ny;
    float f  = 1.0f + CRT_K * r2;
    float dx = nx / f, dy = ny / f;
    float u  = (dx + 1.0f)*0.5f*(SCREEN_W - 1);
    float v  = (1.0f - dy)*0.5f*(SCREEN_H - 1);

    // 2) Animated neon grid background
    const float horizon = SCREEN_H * 0.4f;
    float bg[3];
    if (v < horizon) {
        // sky gradient
        float ty = v / horizon;
        float top[3] = {0.1f,0.0f,0.2f};
        float bot[3] = {1.0f,0.2f,0.5f};
        for (int i = 0; i < 3; ++i)
            bg[i] = top[i]*(1-ty) + bot[i]*ty;
    } else {
        // perspective grid scrolling
        float vy    = (v - horizon) / (SCREEN_H - horizon);
        float depth = 1.0f / vy;
        float wx    = ((u - SCREEN_W*0.5f)/SCREEN_W) * depth * 10.0f;
        float wz    = depth + t * GRID_SCROLL_SPEED;
        float gu = fract_f(wx);
        float gz = fract_f(wz);
        float line = (gu<0.02f||gu>0.98f||gz<0.02f||gz>0.98f) ? 1.0f : 0.0f;
        // neon cyan on dark floor
        bg[0] = 0.02f + line * 0.2f;
        bg[1] = 0.02f + line * 0.8f;
        bg[2] = 0.05f + line * 0.8f;
    }

    // start as background
    out_color[0] = bg[0];
    out_color[1] = bg[1];
    out_color[2] = bg[2];

    // 3) Ball shading
    float bx = game.ball.pos[0], by = game.ball.pos[1];
    float dx_b = u - bx, dy_b = v - by;
    float br   = BALL_SIZE * 0.5f;
    float dsq  = dx_b*dx_b + dy_b*dy_b;
    if (dsq <= br*br) {
        bj_vec3 N = { dx_b/br, dy_b/br, 0.5f };
        bj_vec3_normalize(N,N);
        bj_vec3 L = {0.5f,0.5f,1.0f}; bj_vec3_normalize(L,L);
        float diff = bj_clamp(bj_vec3_dot(N,L), 0.3f, 1.0f);
        out_color[0] = out_color[1] = out_color[2] = diff;
    }

    // 4) Paddle shading & record base colors
    float paddleBase[2][3] = {
        {1.0f,1.0f,0.2f},
        {0.2f,0.2f,1.0f}
    };
    for (int side = 0; side < 2; ++side) {
        float cx = (side==0)
          ? (RACKET_MARGIN + RACKET_WIDTH*0.5f)
          : (SCREEN_W - RACKET_MARGIN - RACKET_WIDTH*0.5f);
        float cy = game.racket[side].y;
        float hw = RACKET_WIDTH*0.5f - PADDLE_CORNER_R;
        float hh = RACKET_LENGTH*0.5f - PADDLE_CORNER_R;
        float rx = fabsf(u - cx) - hw;
        float ry = fabsf(v - cy) - hh;
        if (rx < 0) rx = 0;
        if (ry < 0) ry = 0;
        if (rx*rx + ry*ry <= PADDLE_CORNER_R*PADDLE_CORNER_R) {
            bj_vec3 N = { (u<cx?-1:1)*0.5f, 0.5f, 0.2f };
            bj_vec3_normalize(N,N);
            bj_vec3 L = {0.5f,0.5f,1.0f}; bj_vec3_normalize(L,L);
            float diff = bj_clamp(bj_vec3_dot(N,L)*1.2f, 0.3f, 1.0f);
            out_color[0] = paddleBase[side][0] * diff;
            out_color[1] = paddleBase[side][1] * diff;
            out_color[2] = paddleBase[side][2] * diff;
        }
    }

    // 5) Glow around ball
    {
        float dist_b = sqrtf(dsq);
        float glow_b = 0.0f;
        if (dist_b > br && dist_b < br + GLOW_RANGE_BALL) {
            glow_b = (1.0f - (dist_b - br)/GLOW_RANGE_BALL) * GLOW_INTENSITY;
        }
        if (glow_b > 0.0f) {
            // ball glow color = white
            out_color[0] += glow_b;
            out_color[1] += glow_b;
            out_color[2] += glow_b;
        }
    }

    // 6) Glow around paddles
    for (int side=0; side<2; ++side) {
        float cx = (side==0)
          ? (RACKET_MARGIN + RACKET_WIDTH*0.5f)
          : (SCREEN_W - RACKET_MARGIN - RACKET_WIDTH*0.5f);
        float cy = game.racket[side].y;
        float hw = RACKET_WIDTH*0.5f - PADDLE_CORNER_R;
        float hh = RACKET_LENGTH*0.5f - PADDLE_CORNER_R;
        float rx = fabsf(u - cx) - hw;
        float ry = fabsf(v - cy) - hh;
        if (rx < 0) rx = 0;
        if (ry < 0) ry = 0;
        float extDist = sqrtf(rx*rx + ry*ry);
        if (extDist > 0 && extDist < GLOW_RANGE_PADDLE) {
            float glow_p = (1.0f - extDist/GLOW_RANGE_PADDLE) * GLOW_INTENSITY;
            out_color[0] += paddleBase[side][0] * glow_p;
            out_color[1] += paddleBase[side][1] * glow_p;
            out_color[2] += paddleBase[side][2] * glow_p;
        }
    }

    // 7) Scanlines
    if (((int)y & 1) != 0) {
        out_color[0] *= SCAN_ALPHA;
        out_color[1] *= SCAN_ALPHA;
        out_color[2] *= SCAN_ALPHA;
    }

    // 8) Quantize
    out_color[0] = floorf(out_color[0]*QUANT_LEVELS)/QUANT_LEVELS;
    out_color[1] = floorf(out_color[1]*QUANT_LEVELS)/QUANT_LEVELS;
    out_color[2] = floorf(out_color[2]*QUANT_LEVELS)/QUANT_LEVELS;

    // 9) Brightness boost & clamp
    for (int i = 0; i < 3; ++i) {
        out_color[i] = bj_clamp(out_color[i] * BRIGHTNESS, 0.0f, 1.0f);
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Input & update

static void key_callback(bj_window* w, const bj_key_event* e) {
    (void)w;
    static const struct { bj_key up,down; } map[2] = {
        {BJ_KEY_D,BJ_KEY_F},{BJ_KEY_K,BJ_KEY_J}
    };
    for(int i=0;i<2;++i){
        game.racket[i].up   = (e->key==map[i].up   && e->action==BJ_PRESS);
        game.racket[i].down = (e->key==map[i].down && e->action==BJ_PRESS);
    }
    if(e->key==BJ_KEY_ESCAPE&&e->action==BJ_RELEASE)
        bj_window_set_should_close(w);
    else game.running = BJ_TRUE;
}

static void update_game(double dt) {
    time_secs += (float)dt;
    if (game.running) {
        float half = BALL_SIZE*0.5f;
        bj_vec2 nxt; bj_vec2_add_scaled(nxt, game.ball.pos, game.ball.vel, dt);
        if (nxt[1]>=SCREEN_H-half||nxt[1]<half) game.ball.vel[1]*=-1;
        if (nxt[0]>=SCREEN_W-half||nxt[0]<half) game.ball.vel[0]*=-1;
        bj_vec2_add_scaled(game.ball.pos, game.ball.pos, game.ball.vel, dt);
    }
    for(int i=0;i<2;++i){
        float dir=(game.racket[i].down?1:0)-(game.racket[i].up?1:0);
        float ny=game.racket[i].y + dir*dt*RACKET_VELOCITY;
        float lo=RACKET_LENGTH*0.5f, hi=SCREEN_H-lo;
        if(ny>lo && ny<hi) game.racket[i].y = ny;
    }
}

int bj_app_begin(void** ud, int argc, char* argv[]) {
    (void)ud; (void)argc; (void)argv;
    bj_error* err = NULL;
    if (!bj_begin(&err)) {
        bj_err("Init error 0x%X: %s", err->code, err->message);
        return bj_callback_exit_error;
    }
    window      = bj_window_new("Pong 80s CRT Glow", 0,0,SCREEN_W,SCREEN_H,0);
    framebuffer = bj_window_get_framebuffer(window,0);
    bj_set_key_callback(key_callback);
    return bj_callback_continue;
}

int bj_app_iterate(void* ud) {
    (void)ud;
    bj_dispatch_events();
    update_game(bj_stopwatch_step_delay(&stopwatch));
    bj_bitmap_apply_shader(
        framebuffer,
        pong_rt_shader_crt_80s_glow,
        &time_secs,
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
