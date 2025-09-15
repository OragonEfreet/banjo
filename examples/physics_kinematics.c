////////////////////////////////////////////////////////////////////////////////
/// \example physics_kinematics
/// Colored balls thrown at random angles
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/physics_2d.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;

#define BALLS_LEN 1000
#define BALLS_RADIUS BJ_F(3.0)
#define GRAVITY BJ_F(9.80)

struct {
    bj_vec2 initial_velocity;
    bj_vec2 position;
    uint32_t color;
    double time_alive;
} balls[BALLS_LEN];
bj_vec2 gravity;
bj_vec2 initial_position;
bj_stopwatch stopwatch;

static void reset_ball(size_t at) {
    const uint8_t r = (uint8_t)(128 + rand() % 128);
    const uint8_t g = (uint8_t)(128 + rand() % 128);
    const uint8_t b = (uint8_t)(128 + rand() % 128);

    switch (rand() % 3) {
        case 0: balls[at].color = bj_bitmap_pixel_value(framebuffer, 100, g, b); break;
        case 1: balls[at].color = bj_bitmap_pixel_value(framebuffer, r, 100, b); break;
        default: balls[at].color = bj_bitmap_pixel_value(framebuffer, r, g, 100); break;
    }

    const bj_real angle_rand = (bj_real)rand() / (bj_real)RAND_MAX;
    const bj_real angle = -BJ_PI / BJ_F(2.0) + angle_rand * (BJ_PI / BJ_F(2.0));

    const bj_real magnitude = BJ_F(100.0) + ((bj_real)rand() / (bj_real)RAND_MAX) * BJ_F(100.0);

    bj_vec2_set(
        balls[at].initial_velocity,
        bj_cos(angle) * magnitude,
        bj_sin(angle) * magnitude
    );

    balls[at].time_alive = 0;
}

static void initialize_balls() {

    bj_vec2_set(gravity, 0, GRAVITY);
    bj_vec2_set(initial_position, BALLS_RADIUS + 5, SCREEN_HEIGHT - BALLS_RADIUS - 5);

    for(size_t b = 0 ; b < BALLS_LEN ; ++b) {
        reset_ball(b);
    }

}

static void update(bj_real dt) {
    for(size_t b = 0 ; b < BALLS_LEN ; ++b) {
        balls[b].time_alive += dt;
        bj_kinematics_2d(
            balls[b].position,
            initial_position,
            balls[b].initial_velocity,
            gravity,
            balls[b].time_alive
        );

        const bj_real x = balls[b].position[0];
        const bj_real y = balls[b].position[1];

        if (x + BALLS_RADIUS < BJ_FZERO || x - BALLS_RADIUS > (bj_real)SCREEN_WIDTH ||
            y + BALLS_RADIUS < BJ_FZERO || y - BALLS_RADIUS > (bj_real)SCREEN_HEIGHT
        ) {
            reset_ball(b);
        }


    }
}

static void draw() {
    bj_bitmap_clear(framebuffer);

    for(size_t b = 0 ; b < BALLS_LEN ; ++b) {
        bj_bitmap_draw_filled_circle(framebuffer,
            balls[b].position[0], balls[b].position[1], BALLS_RADIUS,
            balls[b].color
        );
    }
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("2D Kinematics", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    framebuffer = bj_window_get_framebuffer(window, 0);

    initialize_balls();

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

    update(bj_stopwatch_step_delay(&stopwatch));
    draw();
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

