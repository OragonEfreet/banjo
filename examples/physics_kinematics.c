////////////////////////////////////////////////////////////////////////////////
/// \example physics_kinematics.c
/// 2D kinematics for projectile motion simulation.
///
/// Kinematics calculates motion from initial conditions (position, velocity)
/// and constant acceleration (like gravity). Use bj_compute_kinematics_2d() for:
/// - Projectiles: thrown objects, bullets, jumping characters
/// - Falling objects under gravity
/// - Any motion with constant acceleration
///
/// The kinematics equation: position = p₀ + v₀*t + ½*a*t²
/// This gives exact positions at any time t without step-by-step integration.
/// Much more accurate than repeatedly adding velocity each frame.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/physics_2d.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

bj_window* window      = 0;
bj_renderer* renderer  = 0;
bj_bitmap* framebuffer = 0;

#define BALLS_LEN 1000
#define BALLS_RADIUS BJ_F(3.0)
#define GRAVITY BJ_F(50.0)

// Each ball tracks its initial conditions and elapsed time. Kinematics needs:
// - initial_position (shared by all balls here)
// - initial_velocity (unique per ball)
// - acceleration (gravity, shared)
// - time_alive (how long since launch)
// From these, we can compute exact position at any time.
struct {
    bj_vec2 initial_velocity;
    bj_vec2 position;
    uint32_t color;
    double time_alive;
} balls[BALLS_LEN];
bj_vec2 gravity;
bj_vec2 initial_position;
bj_stopwatch stopwatch;

// Setting up a projectile: choose initial velocity based on desired direction
// and speed. For realistic projectile arcs, use angle and magnitude.
// Angle: -90° points straight up, 0° points right (here: -90° to 0° for fountain effect)
// Magnitude: initial speed in pixels/second
static void reset_ball(size_t at) {
    const uint8_t r = (uint8_t)(128 + rand() % 128);
    const uint8_t g = (uint8_t)(128 + rand() % 128);
    const uint8_t b = (uint8_t)(128 + rand() % 128);

    switch (rand() % 3) {
        case 0: balls[at].color = bj_make_bitmap_pixel(framebuffer, 100, g, b); break;
        case 1: balls[at].color = bj_make_bitmap_pixel(framebuffer, r, 100, b); break;
        default: balls[at].color = bj_make_bitmap_pixel(framebuffer, r, g, 100); break;
    }

    // Random angle from -90° to 0° (straight up to straight right).
    const bj_real angle_rand = (bj_real)rand() / (bj_real)RAND_MAX;
    const bj_real angle = -BJ_PI / BJ_F(2.0) + angle_rand * (BJ_PI / BJ_F(2.0));

    // Random speed between 100 and 200 pixels/second.
    const bj_real magnitude = BJ_F(100.0) + ((bj_real)rand() / (bj_real)RAND_MAX) * BJ_F(100.0);

    // Convert angle + magnitude to velocity vector (vx, vy).
    balls[at].initial_velocity.x = bj_cos(angle) * magnitude;
    balls[at].initial_velocity.y = bj_sin(angle) * magnitude;
    balls[at].time_alive = 0;
}

static void initialize_balls() {

    // Gravity acceleration: (0, 50) means 50 pixels/second² downward.
    // Positive y is down in screen coordinates, so gravity pulls objects down.
    // Adjust this value to make physics feel heavier or lighter.
    gravity = (bj_vec2){0, GRAVITY};

    // All balls start from the bottom-left corner.
    initial_position = (bj_vec2){BALLS_RADIUS + 5, SCREEN_HEIGHT - BALLS_RADIUS - 5};

    for(size_t b = 0 ; b < BALLS_LEN ; ++b) {
        reset_ball(b);
    }

}

// Update physics: accumulate time and compute exact positions using kinematics.
// dt is delta time (seconds since last frame) for frame-rate independence.
static void update(bj_real dt) {
    for(size_t b = 0 ; b < BALLS_LEN ; ++b) {
        // Track total time since this ball was launched.
        balls[b].time_alive += dt;

        // bj_compute_kinematics_2d() computes position from:
        //   - initial_position: where the ball started
        //   - initial_velocity: how fast it was moving at launch
        //   - gravity: constant acceleration
        //   - time_alive: elapsed time since launch
        // Returns exact position using: p = p₀ + v₀*t + ½*a*t²
        // This is more accurate than integrating velocity frame-by-frame.
        balls[b].position = bj_compute_kinematics_2d(
            initial_position,
            balls[b].initial_velocity,
            gravity,
            balls[b].time_alive
        );

        const bj_real x = balls[b].position.x;
        const bj_real y = balls[b].position.y;

        // Reset balls that leave the screen.
        if (x + BALLS_RADIUS < BJ_FZERO || x - BALLS_RADIUS > (bj_real)SCREEN_WIDTH ||
            y + BALLS_RADIUS < BJ_FZERO || y - BALLS_RADIUS > (bj_real)SCREEN_HEIGHT
        ) {
            reset_ball(b);
        }


    }
}

static void draw() {
    bj_clear_bitmap(framebuffer);

    for(size_t b = 0 ; b < BALLS_LEN ; ++b) {
        bj_draw_filled_circle(framebuffer,
            balls[b].position.x, balls[b].position.y, BALLS_RADIUS,
            balls[b].color
        );
    }
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;
    srand((unsigned)time(NULL));

    if(!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    } 

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("2D Kinematics", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    framebuffer = bj_get_framebuffer(renderer);

    initialize_balls();

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

    // bj_step_delay_stopwatch() returns delta time (seconds since last call).
    // This ensures physics runs at the same speed regardless of frame rate.
    // Critical for consistent projectile motion across different hardware.
    update(bj_step_delay_stopwatch(&stopwatch));
    draw();
    bj_present(renderer, window);
    bj_sleep(15);

    return bj_should_close_window(window)
         ? bj_callback_exit_success
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    (void)user_data;
    bj_destroy_renderer(renderer);
    bj_unbind_window(window);
    bj_end();
    return status;
}

