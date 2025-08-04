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

////////////////////////////////////////////////////////////////////////////////
// Game data
typedef struct {
    struct {
        bj_vec2 position;
        bj_vec2 velocity;
    } ball;
} pong_game;

////////////////////////////////////////////////////////////////////////////////
// Draw the scene
void draw(const pong_game* game, bj_bitmap* framebuffer) {
    bj_bitmap_clear(framebuffer);

    // Ball
    const uint32_t color_ball = bj_bitmap_pixel_value(framebuffer, 0xFF, 0xFF, 0xFF);

    static bj_rect ball_rect = { .w = BALL_SIZE, .h = BALL_SIZE,};
    ball_rect.x = game->ball.position[0] - BALL_SIZE / 2;
    ball_rect.y = game->ball.position[1] - BALL_SIZE / 2;
    bj_bitmap_draw_rectangle(framebuffer, &ball_rect, color_ball);
}

////////////////////////////////////////////////////////////////////////////////
// Game update
void update(pong_game* game, double delta_time) {

    bj_vec2 target_position;
    bj_vec2_add_scaled(target_position, game->ball.position, game->ball.velocity, delta_time);

    if(target_position[1] >= SCREEN_H || target_position[1] < 0) {
        game->ball.velocity[1] = -(game->ball.velocity[1]);
    }

    if(target_position[0] >= SCREEN_W || target_position[0] < 0) {
        game->ball.velocity[0] = -(game->ball.velocity[0]);
    }
    bj_vec2_add_scaled(game->ball.position, game->ball.position, game->ball.velocity, delta_time);
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

    // Initialize game data
    pong_game game = {
        .ball = {
            .position = { SCREEN_W / 2, SCREEN_H / 2 },
            .velocity = { 200.0f, 200.0f, },
        },
    };

    bj_set_key_callback(bj_close_on_escape);
    bj_stopwatch stopwatch = {0};
    bj_stopwatch_start(&stopwatch);

    // Game loop:
    // - Handle events
    // - Run game loop iteration
    // - Draw scene
    // - Draw scene
    while(bj_window_should_close(window) == BJ_FALSE) {

        bj_dispatch_events();
        update(&game, bj_stopwatch_tick_delay(&stopwatch));
        draw(&game, framebuffer);

        // Flush and wait
        bj_window_update_framebuffer(window);
        bj_sleep(15);
    }

    bj_window_del(window);
    bj_end(0);

    return 0;
}

