////////////////////////////////////////////////////////////////////////////////
/// \example physics_forces
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/assert.h>
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/mat.h>
#include <banjo/physics.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CANVAS_WIDTH 400
#define CANVAS_HEIGHT 300

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;
bj_mat3 projection;

#define PARTICLES_LEN 1
#define PARTICLES_PXL_RADIUS 3

typedef struct {
    // Linear position of the particle in world space
    bj_vec2 position;

    // Holds the linear velocity of the particle in world space
    bj_vec2 velocity;

    // Accumulated force
    bj_vec2 forces;

    // Holds the acceleration of the particle.
    // This value can be used to set acceleration due to gravity 
    // (its primary use) or any other constant acceleration.
    bj_vec2 acceleration;

    // Holds the amount of damping applied to linear motion.
    // Damping is required to remove energy added through numerical instability
    // in the integrator.
    bj_real damping;

    // Holds the inverse of the mass of the particle.
    // It is more useful to hold the inverse mass because integration is
    // simpler and because in real-time simulation it is more useful to
    // have objects with infinite mass(immovable) than zero mass
    // (completely unstable in numerical simulation).
    bj_real inverse_mass;
} particle_t;

particle_t particles[PARTICLES_LEN];

bj_vec2 gravity; // g
bj_stopwatch stopwatch;

static void update_projection() {
    bj_mat3 ortho;
    bj_mat3 viewport;
    bj_mat3_ortho(ortho, -CANVAS_WIDTH/2, CANVAS_WIDTH/2, -CANVAS_HEIGHT/2, CANVAS_HEIGHT/2);
    bj_mat3_viewport(viewport, 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT);
    bj_mat3_mul(projection, viewport, ortho);
}

static void reset_particle(particle_t* p) {
    bj_vec2_set(p->position, BJ_F(0), BJ_F(0));
    bj_vec2_copy(p->acceleration, gravity);
    p->damping = BJ_F(1.0);
}

static void initialize() {
    bj_vec2_set(gravity, BJ_F(0), BJ_F(-9.807));

    for(size_t p = 0 ; p < PARTICLES_LEN ; ++p) {
        reset_particle(&particles[p]);
    }
}


static void add_force(particle_t* p, const bj_vec2 force) {
    bj_vec2_add(p->forces, p->forces, force);
}

// Integrate the particle forward in time by the given amount.
// This function uses a Newton-Euler integration method, which is a linear
// approximation of the correct integral. For this reason it may be inaccurate
// in some cases.
static void integrate_particle(particle_t* part, bj_real dt) {

    bj_particle_integrate_2d(
        part->position,
        part->velocity,
        part->acceleration,
        part->forces,
        part->inverse_mass,
        part->damping,
        dt
    );
        
}

static void update(bj_real dt) {
    for(size_t p = 0 ; p < PARTICLES_LEN ; ++p) {
        integrate_particle(&particles[p], dt);
    }
}

static void draw() {
    bj_bitmap_clear(framebuffer);
    const uint32_t color = bj_bitmap_pixel_value(framebuffer, 0x00, 0xCC, 0x44);

    bj_vec3 c = {0.0, 0.0, 1.0};
    bj_vec3 pc = {0.0, 0.0, 1.0};

    for(size_t p = 0 ; p < PARTICLES_LEN ; ++p) {

        bj_vec2_copy(c, particles[p].position);
        bj_mat3_mul_vec3(pc, projection, c);

        bj_bitmap_draw_filled_circle(framebuffer,
            pc[0], pc[1], BJ_F(PARTICLES_PXL_RADIUS), color
        );
    }

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

    update_projection();
    initialize();

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

