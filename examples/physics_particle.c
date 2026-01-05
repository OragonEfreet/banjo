#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/assert.h>
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/mat.h>
#include <banjo/physics.h>
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
#define CANVAS_WIDTH SCREEN_WIDTH
#define CANVAS_HEIGHT SCREEN_HEIGHT

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;
bj_renderer* renderer  = 0;
bj_mat3x3 projection;

#define G_SUN      BJ_F(120.0)
#define SOFTENING  BJ_F(6.0)
#define M_SUN      BJ_F(1000.0)
#define M_MERCURY  BJ_F(0.055)
#define M_VENUS    BJ_F(0.815)
#define M_EARTH    BJ_F(1.0)
#define M_MARS     BJ_F(0.107)
#define M_JUPITER  BJ_F(317.8)

typedef struct {
    bj_particle_2d body;
    bj_real radius;
    uint32_t color;
} planet_t;

#define N_PLANETS 5
planet_t planets[N_PLANETS];
bj_particle_2d sun = {0};

#define N_ASTEROIDS 800
bj_particle_2d asteroids[N_ASTEROIDS];
uint32_t asteroid_color;

bj_stopwatch stopwatch;

static void update_projection() {
    bj_mat3x3 ortho, viewport;
    bj_mat3_set_ortho(&ortho, -CANVAS_WIDTH/2, CANVAS_WIDTH/2, -CANVAS_HEIGHT/2, CANVAS_HEIGHT/2);
    bj_mat3_set_viewport(&viewport, 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT);
    bj_mat3_mul(&projection, &viewport, &ortho);
}

static bj_real orbital_speed_soft(bj_real G, bj_real M, bj_real r, bj_real eps) {
    const bj_real r2 = r*r;
    const bj_real denom = bj_pow(r2 + eps*eps, BJ_F(1.5));
    return (denom > BJ_FZERO) ? bj_sqrt( (G*M) * r2 / denom ) : BJ_FZERO;
}

static void init_sun() {
    sun.damping = BJ_F(1.0);
    sun.inverse_mass = BJ_F(1.0) / M_SUN;
}

static void init_planet(planet_t* p, bj_real r, bj_real mass, uint32_t color, bj_real draw_r, bj_real phase) {
    const bj_real a = phase;
    p->body.position.x = r * bj_cos(a);
    p->body.position.y = r * bj_sin(a);

    const bj_real v = orbital_speed_soft(G_SUN, M_SUN, r, SOFTENING);
    p->body.velocity.x = -v * bj_sin(a);
    p->body.velocity.y =  v * bj_cos(a);

    p->body.forces = BJ_VEC2_ZERO;
    p->body.damping = BJ_F(1.0);
    p->body.inverse_mass = BJ_F(1.0) / mass;

    p->radius = draw_r;
    p->color = color;
}

static void init_asteroids() {
    asteroid_color = bj_make_bitmap_pixel(framebuffer, 0xB0, 0xB0, 0xB0);
    for (size_t i = 0; i < N_ASTEROIDS; ++i) {
        const bj_real rmin = BJ_F(190.0), rmax = BJ_F(260.0);
        const bj_real t = (bj_real)rand() / (bj_real)RAND_MAX;
        const bj_real u = (bj_real)rand() / (bj_real)RAND_MAX;
        const bj_real r = rmin + (rmax - rmin) * t;
        const bj_real a = BJ_TAU * u;

        asteroids[i].position.x = r * bj_cos(a);
        asteroids[i].position.y = r * bj_sin(a);

        const bj_real v = orbital_speed_soft(G_SUN, M_SUN, r, SOFTENING);
        asteroids[i].velocity.x = -v * bj_sin(a);
        asteroids[i].velocity.y =  v * bj_cos(a);

        asteroids[i].forces = BJ_VEC2_ZERO;
        asteroids[i].damping = BJ_F(1.0);
        asteroids[i].inverse_mass = 1.0;
    }
}

static void initialize() {
    init_sun();

    uint32_t col_mercury = bj_make_bitmap_pixel(framebuffer, 0xC8, 0xC8, 0xC8);
    uint32_t col_venus   = bj_make_bitmap_pixel(framebuffer, 0xD4, 0xA3, 0x58);
    uint32_t col_earth   = bj_make_bitmap_pixel(framebuffer, 0x30, 0xA0, 0xFF);
    uint32_t col_mars    = bj_make_bitmap_pixel(framebuffer, 0xD0, 0x50, 0x30);
    uint32_t col_jupiter = bj_make_bitmap_pixel(framebuffer, 0xD2, 0xB4, 0x8C);

    init_planet(&planets[0], BJ_F(60.0),  M_MERCURY, col_mercury, BJ_F(2.0), BJ_F(0.0));
    init_planet(&planets[1], BJ_F(90.0),  M_VENUS,   col_venus,   BJ_F(3.0), BJ_F(1.2));
    init_planet(&planets[2], BJ_F(130.0), M_EARTH,   col_earth,   BJ_F(3.2), BJ_F(2.0));
    init_planet(&planets[3], BJ_F(170.0), M_MARS,    col_mars,    BJ_F(2.6), BJ_F(2.6));
    init_planet(&planets[4], BJ_F(260.0), M_JUPITER, col_jupiter, BJ_F(6.0), BJ_F(0.8));

    init_asteroids();
}

static void update(bj_real t) { (void)t; }

#define DT_CLAMP (BJ_F(1.0)/BJ_F(120.0))

static void physics(bj_real dt) {
    if (dt > DT_CLAMP) dt = DT_CLAMP;

    for (size_t i = 0; i < N_PLANETS; ++i) {
        bj_apply_point_gravity_softened_2d(&planets[i].body, &sun, G_SUN, SOFTENING);
        bj_step_particle_2d(&planets[i].body, dt);
    }
    for (size_t i = 0; i < N_ASTEROIDS; ++i) {
        bj_apply_point_gravity_softened_2d(&asteroids[i], &sun, G_SUN, SOFTENING);
        bj_step_particle_2d(&asteroids[i], dt);
    }
}

static void draw() {
    bj_clear_bitmap(framebuffer);

    const uint32_t col_sun = bj_make_bitmap_pixel(framebuffer, 0xFF, 0xCC, 0x44);

    bj_vec3 c = { sun.position.x, sun.position.y, BJ_F(1.0) };
    bj_vec3 pc = bj_mat3_transform_vec3(&projection, c);
    bj_draw_filled_circle(framebuffer, pc.x, pc.y, BJ_F(10.0), col_sun);

    for (size_t i = 0; i < N_PLANETS; ++i) {
        c.x = planets[i].body.position.x; c.y = planets[i].body.position.y;
        pc = bj_mat3_transform_vec3(&projection, c);
        bj_draw_filled_circle(framebuffer, pc.x, pc.y, planets[i].radius, planets[i].color);
    }

    for (size_t i = 0; i < N_ASTEROIDS; ++i) {
        c.x = asteroids[i].position.x; c.y = asteroids[i].position.y;
        pc = bj_mat3_transform_vec3(&projection, c);
        bj_put_pixel(framebuffer, (int)pc.x, (int)pc.y, asteroid_color);
    }
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    srand((unsigned)time(NULL));

    bj_error* p_error = 0;
    if (!bj_begin(BJ_VIDEO_SYSTEM, &p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    window = bj_bind_window("2D Solar System + Asteroids", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    bj_renderer_configure(renderer, window);
    bj_set_key_callback(bj_close_on_escape, 0);

    framebuffer = bj_get_framebuffer(renderer);

    update_projection();
    initialize();

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

    update(bj_stopwatch_elapsed(&stopwatch));
    physics(bj_stopwatch_delay(&stopwatch));
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
