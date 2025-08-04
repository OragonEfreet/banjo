////////////////////////////////////////////////////////////////////////////////
/// \example drawing_2d.c
///  2D primitive drawing demo.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
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

// Ball data
bj_vec2 ball_position = { SCREEN_W / 2, SCREEN_H / 2 };
bj_vec2 ball_velocity = { 200.0f, 200.0f, };

bj_stopwatch stopwatch = {0};
bj_window* window      = 0;
bj_bitmap* framebuffer = 0;

////////////////////////////////////////////////////////////////////////////////
// Draw the scene
void draw(bj_bitmap* bmp) {
    bj_bitmap_clear(bmp);

    const uint32_t color_ball = bj_bitmap_pixel_value(bmp, 0xFF, 0xFF, 0xFF);

    bj_bitmap_draw_rectangle(framebuffer,
        &(bj_rect){
            .x = ball_position[0] - BALL_SIZE / 2, 
            .y = ball_position[1] - BALL_SIZE / 2,
            .w = BALL_SIZE, 
            .h = BALL_SIZE,
        },
        color_ball
    );
}

////////////////////////////////////////////////////////////////////////////////
// Game update
void update(double delta_time) {

    bj_vec2 target_position;
    bj_vec2_add_scaled(target_position, ball_position, ball_velocity, delta_time);

    if(target_position[1] >= SCREEN_H || target_position[1] < 0) {
        ball_velocity[1] = -ball_velocity[1];
    }

    if(target_position[0] >= SCREEN_W || target_position[0] < 0) {
        ball_velocity[0] = -ball_velocity[0];
    }
    bj_vec2_add_scaled(ball_position, ball_position, ball_velocity, delta_time);
}


////////////////////////////////////////////////////////////////////////////////
// Initialization
int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if (!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    window      = bj_window_new("Pong", 100, 100, SCREEN_W, SCREEN_H, 0);
    framebuffer = bj_window_get_framebuffer(window, 0);

    bj_set_key_callback(bj_close_on_escape);

    bj_stopwatch_start(&stopwatch);
    return bj_callback_continue;
}

////////////////////////////////////////////////////////////////////////////////
// Game loop iteration
// - Handle events
// - Update game
// - Draw
int bj_app_iterate(void* user_data) {
    (void)user_data;

    bj_dispatch_events();
    update(bj_stopwatch_tick_delay(&stopwatch));
    draw(framebuffer);

    bj_window_update_framebuffer(window);
    bj_sleep(15);

    return bj_window_should_close(window)
        ? bj_callback_exit_success
        : bj_callback_continue;
}

////////////////////////////////////////////////////////////////////////////////
// Game termination
int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_window_del(window);
    bj_end(0);
    return status;
}

