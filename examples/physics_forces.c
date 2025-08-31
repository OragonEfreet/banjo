////////////////////////////////////////////////////////////////////////////////
/// \example physics_forces
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/physics.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;

#define PARTICLES_LEN 1000
#define PARTICLES_RADIUS 3
#define GRAVITY BJ_F(9.80)

struct {
    int novalue;
} particles[PARTICLES_LEN];
bj_vec2 gravity;
bj_stopwatch stopwatch;

static void reset_particle(size_t at) {
    (void)at;

}

static void initialize_particles() {

    for(size_t p = 0 ; p < PARTICLES_LEN ; ++p) {
        reset_particle(p);
    }

}

static void update(bj_real dt) {
    (void)dt;

}

static void draw() {
    bj_bitmap_clear(framebuffer);

    /* for(size_t b = 0 ; b < PARTICLES_LEN ; ++b) { */
    /*     bj_bitmap_draw_filled_circle(framebuffer, */
    /*         particles[b].position[0], particles[b].position[1], BJ_F(PARTICLES_RADIUS), */
    /*         particles[b].color */
    /*     ); */
    /* } */
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("2D Kinematics", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    framebuffer = bj_window_get_framebuffer(window, 0);

    initialize_particles();

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

