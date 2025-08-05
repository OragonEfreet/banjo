// pong.c
// Pong clone with toggleable rectangle draw vs. CRT-raytraced shader.
// Press SPACE to switch modes.

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
#include <banjo/math.h>    // bj_clamp, bj_smoothstep, bj_fract

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Compile-time settings
#define SCREEN_W           800
#define SCREEN_H           600
#define BALL_SIZE          16.0f
#define PADDLE_MARGIN      50.0f
#define PADDLE_LENGTH      120.0f
#define PADDLE_WIDTH       24.0f
#define PADDLE_VELOCITY    250.0f
#define LEFT_PADDLE_POSX   (PADDLE_MARGIN)
#define RIGHT_PADDLE_POSX  (SCREEN_W - PADDLE_MARGIN - PADDLE_WIDTH)

// CRT & chroma constants
#define CRT_K               0.03f    // barrel distortion strength
#define CHROMA_OFFSET       0.005f   // per-channel distortion delta
#define FLICKER_FREQ        60.0f    // Hz
#define NOISE_INTENSITY     0.02f

////////////////////////////////////////////////////////////////////////////////
// Game state
static struct {
    struct { bj_vec2 position, velocity; } ball;
    struct { float position_y; bj_bool up, down; } paddle[2];
    bj_bool running;
} game;

// Toggle between rectangle draw and shader draw
static bj_bool use_shader = BJ_FALSE;

// Banjo plumbing
static bj_window*   window      = NULL;
static bj_bitmap*   framebuffer = NULL;
static bj_stopwatch stopwatch    = {0};
static float       time_secs    = 0.0f;

////////////////////////////////////////////////////////////////////////////////
// Original rectangle draw
static void draw_rectangles(bj_bitmap* fb) {
    bj_bitmap_clear(fb);
    uint32_t c = bj_bitmap_pixel_value(fb, 0xFF,0xFF,0xFF);

    // Middle dashed line
    int block = SCREEN_H/12;
    int dash = block*2/3;
    int gap  = block/3;
    int count = (SCREEN_H + gap)/block;
    int startY = (SCREEN_H - (count*dash + (count-1)*gap))/2;
    int xmid = SCREEN_W/2 - ((SCREEN_W/400)+1)/2;
    int thickness = (SCREEN_W/400)+1;
    for(int i=0;i<count;++i){
        bj_rect r = {
            .x = xmid,
            .y = startY + i*block,
            .w = thickness,
            .h = dash
        };
        bj_bitmap_draw_rectangle(fb,&r,c);
    }

    // Ball
    {
        bj_rect br = {.w = BALL_SIZE, .h = BALL_SIZE};
        br.x = (int)(game.ball.position[0] - BALL_SIZE/2);
        br.y = (int)(game.ball.position[1] - BALL_SIZE/2);
        bj_bitmap_draw_rectangle(fb,&br,c);
    }

    // Paddles
    for(int p=0;p<2;++p){
        bj_rect pr = {
            .x = (p==0?LEFT_PADDLE_POSX:RIGHT_PADDLE_POSX),
            .w = PADDLE_WIDTH,
            .h = PADDLE_LENGTH
        };
        pr.y = (int)(game.paddle[p].position_y - PADDLE_LENGTH/2);
        bj_bitmap_draw_rectangle(fb,&pr,c);
    }
}

////////////////////////////////////////////////////////////////////////////////
// SDF primitives for shader
static float sdf_circle(float dx, float dy, float r) {
    return sqrtf(dx*dx+dy*dy) - r;
}
static float sdf_rect(float dx, float dy, float hw, float hh) {
    float ax = fabsf(dx)-hw, ay = fabsf(dy)-hh;
    float ox = fmaxf(ax,0.0f), oy = fmaxf(ay,0.0f);
    return sqrtf(ox*ox+oy*oy) + fminf(fmaxf(ax,ay),0.0f);
}
static float eval_cover(float u, float v) {
    float c=0.0f;
    // middle line
    {
        float mx=SCREEN_W*0.5f, mw=2.0f;
        c = fmaxf(c, 1.0f - bj_smoothstep(0.0f,mw,fabsf(u-mx)));
    }
    // ball
    {
        float bx=game.ball.position[0], by=game.ball.position[1];
        float d=sdf_circle(u-bx,v-by,BALL_SIZE*0.5f);
        c=fmaxf(c,1.0f-bj_smoothstep(0.0f,1.0f,d));
    }
    // paddles
    for(int i=0;i<2;++i){
        float px=(i==0?LEFT_PADDLE_POSX:RIGHT_PADDLE_POSX)+PADDLE_WIDTH*0.5f;
        float py=game.paddle[i].position_y;
        float d=sdf_rect(u-px,v-py,PADDLE_WIDTH*0.5f,PADDLE_LENGTH*0.5f);
        c=fmaxf(c,1.0f-bj_smoothstep(0.0f,1.0f,d));
    }
    return c;
}

////////////////////////////////////////////////////////////////////////////////
// Shader draw
static int pong_shader_crt_rgb(bj_vec3 out_color, const bj_vec2 frag, void* _ud) {
    float t = *(float*)_ud;
    // normalize to [-1,+1]
    float nx=2.0f*frag[0]/(SCREEN_W-1)-1.0f;
    float ny=1.0f-2.0f*frag[1]/(SCREEN_H-1);
    float r2=nx*nx+ny*ny;
    // per-channel barrel
    float k=CRT_K, d=CHROMA_OFFSET;
    float fR=1.0f+(k+d)*r2, fG=1.0f+k*r2, fB=1.0f+(k-d)*r2;
    float dxR=nx/fR, dyR=ny/fR, dxG=nx/fG, dyG=ny/fG, dxB=nx/fB, dyB=ny/fB;
    float uR=(dxR+1)*0.5f*(SCREEN_W-1), vR=(1-dyR)*0.5f*(SCREEN_H-1);
    float uG=(dxG+1)*0.5f*(SCREEN_W-1), vG=(1-dyG)*0.5f*(SCREEN_H-1);
    float uB=(dxB+1)*0.5f*(SCREEN_W-1), vB=(1-dyB)*0.5f*(SCREEN_H-1);
    float covR=eval_cover(uR,vR), covG=eval_cover(uG,vG), covB=eval_cover(uB,vB);
    out_color[0]=covR; out_color[1]=covG; out_color[2]=covB;
    // softened vignette
    {
        float vig=1.0f-bj_smoothstep(0.7f,1.0f,r2)*0.5f;
        out_color[0]*=vig; out_color[1]*=vig; out_color[2]*=vig;
    }
    // scanlines
    {
        int band = ((int)frag[1])&3;
        float lm=(band==0||band==3?0.6f:1.0f);
        out_color[0]*=lm; out_color[1]*=lm; out_color[2]*=lm;
    }
    // flicker
    {
        float flick=1.0f+0.03f*sinf(2*M_PI*FLICKER_FREQ*t+frag[1]*0.1f);
        out_color[0]*=flick; out_color[1]*=flick; out_color[2]*=flick;
    }
    // noise
    {
        float n=bj_fract(sinf(frag[0]*12.9898f+frag[1]*78.233f+t*5.0f)*43758.5453f)-0.5f;
        n*=NOISE_INTENSITY;
        out_color[0]+=n; out_color[1]+=n; out_color[2]+=n;
    }
    // clamp
    out_color[0]=bj_clamp(out_color[0],0,1);
    out_color[1]=bj_clamp(out_color[1],0,1);
    out_color[2]=bj_clamp(out_color[2],0,1);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// draw dispatcher
static void draw(bj_bitmap* fb) {
    if (use_shader) {
        bj_bitmap_apply_shader(fb, pong_shader_crt_rgb, &time_secs, 0);
    } else {
        draw_rectangles(fb);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Input & game logic
static void key_callback(bj_window* w, const bj_key_event* e) {
    (void)w;
    static const struct { bj_key up, down; } map[2] = {
        { BJ_KEY_D, BJ_KEY_F }, { BJ_KEY_K, BJ_KEY_J }
    };
    int i;
    for (i = 0; i < 2; ++i) {
        if (e->key == map[i].up) {
            game.paddle[i].up   = (e->action == BJ_PRESS);
            game.running = BJ_TRUE;
        }
        if (e->key == map[i].down) {
            game.paddle[i].down = (e->action == BJ_PRESS);
            game.running = BJ_TRUE;
        }
    }
    if (e->key == BJ_KEY_ESCAPE && e->action == BJ_RELEASE) {
        bj_window_set_should_close(w);
    } else if (e->key == BJ_KEY_SPACE && e->action == BJ_PRESS) {
        use_shader = !use_shader;
    }
}

static void reset_game(void) {
    bj_vec2_set(game.ball.position, SCREEN_W*0.5f, SCREEN_H*0.5f);
    bj_vec2_set(game.ball.velocity, 200.0f, 200.0f);
    int i;
    for (i = 0; i < 2; ++i) {
        game.paddle[i].position_y = SCREEN_H*0.5f;
        game.paddle[i].up = BJ_FALSE;
        game.paddle[i].down = BJ_FALSE;
    }
    game.running = BJ_FALSE;
}

static void update_game(double dt) {
    float half = BALL_SIZE * 0.5f;
    bj_vec2 tp;
    if (game.running) {
        bj_vec2_add_scaled(tp, game.ball.position, game.ball.velocity, dt);
        /* wall bounce */
        if (tp[1] < half || tp[1] > SCREEN_H-half) {
            tp[1] = bj_clamp(tp[1], half, SCREEN_H-half);
            game.ball.velocity[1] *= -1.0f;
        }
        /* paddle bounce */
        {
            float speed = bj_vec2_len(game.ball.velocity);
            int p;
            for (p = 0; p < 2; ++p) {
                float cx = (p == 0 ? LEFT_PADDLE_POSX : RIGHT_PADDLE_POSX)
                         + PADDLE_WIDTH * 0.5f;
                if (fabsf(tp[0]-cx) <= half + PADDLE_WIDTH*0.5f &&
                    fabsf(tp[1]-game.paddle[p].position_y) <=
                      half + PADDLE_LENGTH*0.5f)
                {
                    float base = (p==0?0.0f:BJ_PI);
                    float a = ((rand()/(float)RAND_MAX)*2.0f-1.0f)
                              *(BJ_PI/4.0f) + base;
                    bj_vec2_set(game.ball.velocity,
                                cosf(a)*speed, sinf(a)*speed);
                    break;
                }
            }
        }
        bj_vec2_copy(game.ball.position, tp);
        /* score reset */
        if (game.ball.position[0] < -half) {
            bj_info("Right scores"); reset_game();
        }
        if (game.ball.position[0] > SCREEN_W+half) {
            bj_info("Left scores"); reset_game();
        }
    }
    /* paddle movement */
    {
        int p;
        for (p = 0; p < 2; ++p) {
            float dir = (float)game.paddle[p].down
                      - (float)game.paddle[p].up;
            float ny = game.paddle[p].position_y
                     + dir * dt * PADDLE_VELOCITY;
            game.paddle[p].position_y =
                bj_clamp(ny,
                         PADDLE_LENGTH*0.5f,
                         SCREEN_H-PADDLE_LENGTH*0.5f);
        }
    }
}

int bj_app_begin(void** ud, int argc, char* argv[]) {
    (void)ud; (void)argc; (void)argv;
    bj_error* err = NULL;
    if (!bj_begin(&err)) {
        bj_err("Init error 0x%X: %s", err->code, err->message);
        return bj_callback_exit_error;
    }
    srand((unsigned)time(NULL));
    window      = bj_window_new("Pong Toggle CRT", 0,0,
                                SCREEN_W, SCREEN_H, 0);
    framebuffer = bj_window_get_framebuffer(window, 0);
    bj_set_key_callback(key_callback);
    reset_game();
    return bj_callback_continue;
}

int bj_app_iterate(void* ud) {
    (void)ud;
    bj_dispatch_events();
    {
        double dt = bj_stopwatch_step_delay(&stopwatch);
        time_secs += (float)dt;
        update_game(dt);
    }
    draw(framebuffer);
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
