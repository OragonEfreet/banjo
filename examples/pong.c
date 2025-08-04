////////////////////////////////////////////////////////////////////////////////
/// \example pong.c
/// A clone of Pong.
////////////////////////////////////////////////////////////////////////////////
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

#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
// Compile time, customize before building.
#define SCREEN_W 800
#define SCREEN_H 600
#define BALL_SIZE 16
#define PADDLE_MARGIN 50
#define PADDLE_LENGTH 120
#define PADDLE_WIDTH 24
#define PADDLE_VELOCITY (250.f)
#define GAME_START_DELAY (1.f)
#define LEFT_PADDLE_POSX (PADDLE_MARGIN)
#define RIGHT_PADDLE_POSX (SCREEN_W - PADDLE_MARGIN - PADDLE_WIDTH)

////////////////////////////////////////////////////////////////////////////////
// Game data
static struct {
    struct {
        bj_vec2 position;
        bj_vec2 velocity;
    } ball;

    struct {
        float   position_y;
        bj_bool up;
        bj_bool down;
    } paddle[2];
    bj_bool running;
} game = {
    .ball = {
        .position = { SCREEN_W / 2, SCREEN_H / 2 },
        .velocity = { 200.0f, 200.0f, },
    },
    .paddle = { 
        {.position_y = SCREEN_H / 2, .up = BJ_FALSE, .down = BJ_FALSE},
        {.position_y = SCREEN_H / 2, .up = BJ_FALSE, .down = BJ_FALSE}
    },
    .running = BJ_FALSE,
};

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;
bj_stopwatch stopwatch = {0};

////////////////////////////////////////////////////////////////////////////////
// Draw the scene
// Important: bj_bitmap_draw_rect draws from the top-left corner of the rect
// while the game's entities (paddles, ball) have coordinates on their centers.
void draw(bj_bitmap* framebuffer) {
    bj_bitmap_clear(framebuffer);

    const uint32_t color = bj_bitmap_pixel_value(framebuffer, 0xFF, 0xFF, 0xFF);

    // Ball
    static bj_rect ball_rect = { .w = BALL_SIZE, .h = BALL_SIZE,};
    ball_rect.x = game.ball.position[0] - BALL_SIZE / 2;
    ball_rect.y = game.ball.position[1] - BALL_SIZE / 2;
    bj_bitmap_draw_rectangle(framebuffer, &ball_rect, color);

    static bj_rect paddle_rect[2] = {
        {.x = LEFT_PADDLE_POSX, .w = PADDLE_WIDTH, .h = PADDLE_LENGTH,},
        {.x = RIGHT_PADDLE_POSX, .w = PADDLE_WIDTH, .h = PADDLE_LENGTH,},
    };

    for(size_t r = 0 ; r < 2 ; ++r) {
        paddle_rect[r].y = game.paddle[r].position_y - PADDLE_LENGTH / 2;
        bj_bitmap_draw_rectangle(framebuffer, &paddle_rect[r], color);
    }

}

void key_callback(bj_window* p_window, const bj_key_event* e) {
    (void)p_window;

    static const struct {
        bj_key up;
        bj_key down;
    } keymap[2] = {
        {.up = BJ_KEY_D, .down = BJ_KEY_F},
        {.up = BJ_KEY_K, .down = BJ_KEY_J},
    };

    for(size_t r = 0 ; r < 2 ; ++r) {
        if(e->key == keymap[r].up) {
            game.paddle[r].up = e->action == BJ_PRESS;
        }
        if(e->key == keymap[r].down) {
            game.paddle[r].down = e->action == BJ_PRESS;
        }
    }

    if(e->key == BJ_KEY_ESCAPE && e->action == BJ_RELEASE) {
        bj_window_set_should_close(p_window);
    } else {
        game.running = BJ_TRUE;
    }
}


////////////////////////////////////////////////////////////////////////////////
// Game update
static inline float clampf(float x, float lo, float hi) {
    return x < lo ? lo : x > hi ? hi : x;
}

void update(double dt) {
    const float halfB = BALL_SIZE * 0.5f;
    bj_vec2 tp;
    bj_vec2_add_scaled(tp, game.ball.position, game.ball.velocity, dt);

    // wall bounce (top/bottom)
    if (tp[1] < halfB || tp[1] > SCREEN_H - halfB) {
        tp[1] = clampf(tp[1], halfB, SCREEN_H - halfB);
        game.ball.velocity[1] = -game.ball.velocity[1];
    }

    // paddle bounce with random angle
    float speed = bj_vec2_len(game.ball.velocity);
    for (int p = 0; p < 2; p++) {
        float cx = (p == 0 ? LEFT_PADDLE_POSX : RIGHT_PADDLE_POSX) + PADDLE_WIDTH * 0.5f;
        if (fabsf(tp[0] - cx) <= halfB + PADDLE_WIDTH * 0.5f &&
            fabsf(tp[1] - game.paddle[p].position_y) <= halfB + PADDLE_LENGTH * 0.5f) 
        {
            // random angle away from paddle: base 0 (left) or π (right) ±45°
            float base = p == 0 ? 0.0f : BJ_PI;
            float randn = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
            float angle = base + randn * (BJ_PI / 4.0f);
            game.ball.velocity[0] = cosf(angle) * speed;
            game.ball.velocity[1] = sinf(angle) * speed;
            break;
        }
    }

    bj_vec2_copy(game.ball.position, tp);

    // paddle movement
    for (int p = 0; p < 2; p++) {
        float dir = (float)game.paddle[p].down - (float)game.paddle[p].up;
        float y  = game.paddle[p].position_y + dir * dt * PADDLE_VELOCITY;
        game.paddle[p].position_y = clampf(y, PADDLE_LENGTH * 0.5f, SCREEN_H - PADDLE_LENGTH * 0.5f);
    }
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window      = bj_window_new("Pong", 0,0, SCREEN_W, SCREEN_H, 0);
    framebuffer = bj_window_get_framebuffer(window, 0);

    bj_set_key_callback(key_callback);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();
    update(bj_stopwatch_step_delay(&stopwatch));

    draw(framebuffer);
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

