////////////////////////////////////////////////////////////////////////////////
/// \example pong.c
/// A clone of Pong.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/bitmap.h>
#include <banjo/event.h>
#include <banjo/linmath.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

////////////////////////////////////////////////////////////////////////////////
// Compile time, customize before building.
#define SCREEN_W 800
#define SCREEN_H 600
#define BALL_SIZE 16
#define RACKET_MARGIN 10
#define RACKET_LENGTH 120
#define RACKET_WIDTH 24
#define RACKET_VELOCITY (250.f)
#define GAME_START_DELAY (1.f)

////////////////////////////////////////////////////////////////////////////////
// Game data
static struct {
    struct {
        bj_vec2 position;
        bj_vec2 velocity;
    } ball;

    struct {
        float position_y;
        bj_bool up;
        bj_bool down;
    } racket[2];
    bj_stopwatch start;
} game = {
    .ball = {
        .position = { SCREEN_W / 2, SCREEN_H / 2 },
        .velocity = { 200.0f, 200.0f, },
    },
    .racket = { 
        {.position_y = SCREEN_H / 2, .up = BJ_FALSE, .down = BJ_FALSE},
        {.position_y = SCREEN_H / 2, .up = BJ_FALSE, .down = BJ_FALSE}
    },
    .start = 0,
};

////////////////////////////////////////////////////////////////////////////////
// Draw the scene
// Important: bj_bitmap_draw_rect draws from the top-left corner of the rect
// while the game's entities (rackets, ball) have coordinates on their centers.
void draw(bj_bitmap* framebuffer) {
    bj_bitmap_clear(framebuffer);

    const uint32_t color = bj_bitmap_pixel_value(framebuffer, 0xFF, 0xFF, 0xFF);

    // Ball
    static bj_rect ball_rect = { .w = BALL_SIZE, .h = BALL_SIZE,};
    ball_rect.x = game.ball.position[0] - BALL_SIZE / 2;
    ball_rect.y = game.ball.position[1] - BALL_SIZE / 2;
    bj_bitmap_draw_rectangle(framebuffer, &ball_rect, color);

    static bj_rect racket_rect[2] = {
        {.x = RACKET_MARGIN, .w = RACKET_WIDTH, .h = RACKET_LENGTH,},
        {.x = SCREEN_W - RACKET_MARGIN - RACKET_WIDTH, .w = RACKET_WIDTH, .h = RACKET_LENGTH,},
    };

    for(size_t r = 0 ; r < 2 ; ++r) {
        racket_rect[r].y = game.racket[r].position_y - RACKET_LENGTH / 2;
        bj_bitmap_draw_rectangle(framebuffer, &racket_rect[r], color);
    }

}

void key_callback(bj_window* p_window, const bj_key_event* e) {
    (void)p_window;

    switch(e->key) {
        case BJ_KEY_F:
            game.racket[0].down = (e->action == BJ_PRESS);
            break;
        case BJ_KEY_D:
            game.racket[0].up = (e->action == BJ_PRESS);
            break;
        case BJ_KEY_J:
            game.racket[1].down = (e->action == BJ_PRESS);
            break;
        case BJ_KEY_K:
            game.racket[1].up = (e->action == BJ_PRESS);
            break;
        case BJ_KEY_ESCAPE:
            if(e->action == BJ_RELEASE) {
                bj_window_set_should_close(p_window);
            }
            break;
        default:
            break;

        
    }



}

////////////////////////////////////////////////////////////////////////////////
// Game update
void update(double delta_time) {

    // Ball
    bj_stopwatch_tick(&game.start);
    if(bj_stopwatch_elapsed(&game.start) > GAME_START_DELAY) {
        bj_vec2 target_position;
        bj_vec2_add_scaled(target_position, game.ball.position, game.ball.velocity, delta_time);

        if(target_position[1] >= SCREEN_H || target_position[1] < 0) {
            game.ball.velocity[1] = -(game.ball.velocity[1]);
        }

        if(target_position[0] >= SCREEN_W || target_position[0] < 0) {
            game.ball.velocity[0] = -(game.ball.velocity[0]);
        }
        bj_vec2_add_scaled(game.ball.position, game.ball.position, game.ball.velocity, delta_time);
    }

    // Paddle
    for(size_t r = 0 ; r < 2 ; ++r) {
        const float new_y = game.racket[r].position_y + 
            (1.f * game.racket[r].down - 1.f * game.racket[r].up) 
            * delta_time * RACKET_VELOCITY;

        if(new_y > RACKET_LENGTH / 2 && new_y < SCREEN_H - RACKET_LENGTH / 2) {
            game.racket[r].position_y = new_y;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    bj_error* p_error = 0;

    if (!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return -1;
    }

    bj_window* window      = bj_window_new("Pong", 0, 0, SCREEN_W, SCREEN_H, 0);
    bj_bitmap* framebuffer = bj_window_get_framebuffer(window, 0);

    bj_set_key_callback(key_callback);
    bj_stopwatch stopwatch = {0};
    bj_stopwatch_start(&stopwatch);

    bj_stopwatch_start(&game.start);

    // Game loop:
    // - Handle events
    // - Run game loop iteration
    // - Draw scene
    // - Draw scene
    while(bj_window_should_close(window) == BJ_FALSE) {

        bj_dispatch_events();
        update(bj_stopwatch_tick_delay(&stopwatch));
        draw(framebuffer);

        // Flush and wait
        bj_window_update_framebuffer(window);
        bj_sleep(15);
    }

    bj_window_del(window);
    bj_end(0);

    return 0;
}

