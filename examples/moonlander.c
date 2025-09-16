#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/assert.h>
#include <banjo/audio.h>
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/mat.h>
#include <banjo/math.h>
#include <banjo/physics.h>
#include <banjo/physics_2d.h>
#include <banjo/shader.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#include <math.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define CANVAS_W 80
#define CANVAS_H 60

#define LANDER_VERTICES_LEN 22
#define LANDER_EDGES_LEN 25

////////////////////////////////////////////////////////////////////////////////
/// Game Data
typedef struct {
    bj_rigid_body_2d body;

    bj_real drag_k1;
    bj_real drag_k2;

    struct {
        bj_bool left;
        bj_bool right;
        bj_bool up;
        bj_real magnitude;
    } thrusters;
} lander;

typedef struct {
    bj_window*   window;
    bj_stopwatch stopwatch;

    struct {
        bj_real g;
    } world;

    lander lander;

    struct {
        bj_bitmap*   framebuffer;
        struct { float radius; float angle;} coords[LANDER_VERTICES_LEN];
        size_t edges[LANDER_EDGES_LEN][2];
        bj_mat3 projection;
    } draw;

} game_data;

///////////////////////////////////////////////////////////////////////////////
/// Drawing
static void prepare_assets(game_data* data) {

    const float lander_coords_m[][2] = {
        {-3,  11}, {-7,  7}, {-7,  1}, {-3, -3},  { 3, -3},
        { 7,  1},  { 7,  7}, { 3, 11}, {-8, -4},  {-8, -9},
        { 8, -9},  { 8, -4}, {-3, -4}, { 3, -4},  {-4, -9},
        {-7, -12}, { 7, -12}, { 4, -9}, {-13, -13}, {-10, -13},
        {10, -13}, {13, -13},
    };

    const size_t lander_edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 0}, 
        {8, 9}, {9, 10}, {10, 11}, {11, 8},  
        {3, 12}, {4, 13},
        {8, 18}, {18, 19}, {9, 19}, {9, 18},  
        {11, 21}, {20, 21}, {10, 20}, {10, 21}, 
        {14, 15}, {15, 16}, {16, 17},
    };

    // To have more realistic sizes , I consider that the 
    // full height should be 7 meters in total.
    // I compute the full Y dimension of the above coordinates, and infer a "to 7" ratio
    // that I will apply to every radius values while converting to polar coordinates.
    int min_y = 0, max_y = 0;
    for(size_t c = 0 ; c < LANDER_VERTICES_LEN ; ++c) {
        const float y = lander_coords_m[c][1];
        if(y < min_y) min_y = y;
        if(y > max_y) max_y = y;
    }
    const float full_height = max_y - min_y;
    const float size_ratio = 7.f / full_height;

    for(size_t c = 0 ; c < LANDER_VERTICES_LEN ; ++c) {
        const float x = lander_coords_m[c][0];
        const float y = lander_coords_m[c][1];
        data->draw.coords[c].radius = sqrt(x * x + y * y) * size_ratio;
        data->draw.coords[c].angle  = atan2f(y, x);
    }

    bj_memcpy(data->draw.edges, lander_edges, sizeof(lander_edges));
}

static void update_projection(game_data* data) {
    bj_mat3 ortho;
    bj_mat3 viewport;
    bj_mat3_ortho(ortho, -CANVAS_W, CANVAS_W, -CANVAS_H, CANVAS_H);
    bj_mat3_viewport(viewport, 0.f, 0.f, SCREEN_W, SCREEN_H);
    bj_mat3_mul(data->draw.projection, viewport, ortho);
}

static void draw(game_data* data) {
    bj_bitmap* target = data->draw.framebuffer;

    const uint32_t color = bj_make_bitmap_pixel(target, 0x00, 0xCC, 0x44);
    bj_clear_bitmap(target);

    const float x = data->lander.body.particle.position[0];
    const float y = data->lander.body.particle.position[1];

    bj_vec3 p0, q0;
    bj_vec3 p1, q1;

    for (size_t e = 0; e < LANDER_EDGES_LEN; ++e) {
        const float r0 = data->draw.coords[data->draw.edges[e][0]].radius;
        const float a0 = data->draw.coords[data->draw.edges[e][0]].angle  + data->lander.body.angular.value;
        const float r1 = data->draw.coords[data->draw.edges[e][1]].radius;
        const float a1 = data->draw.coords[data->draw.edges[e][1]].angle  + data->lander.body.angular.value;

        bj_vec3_set(q0, bj_cosf(a0) * r0 + x, bj_sinf(a0) * r0 + y, 1.f);
        bj_vec3_set(q1, bj_cosf(a1) * r1 + x, bj_sinf(a1) * r1 + y, 1.f);

        bj_mat3_mul_vec3(p0, data->draw.projection, q0);
        bj_mat3_mul_vec3(p1, data->draw.projection, q1);

        bj_draw_line(
            target,
            p0[0], p0[1], 
            p1[0], p1[1], 
            color
        );
    }

    // UI text
    const unsigned size = 18;
    const uint32_t white = bj_make_bitmap_pixel(target, 0xAA, 0xAA, 0xAA);
    bj_draw_textf(target, 10, SCREEN_H - 10 - size, size, white, "up: %s  left: %s  right: %s", 
        data->lander.thrusters.up == BJ_TRUE ? "\x1B[32mYES\x1B[0m" : " \x1B[31mno\x1B[0m",
        data->lander.thrusters.left == BJ_TRUE ? "\x1B[32mYES\x1B[0m" : " \x1B[31mno\x1B[0m",
        data->lander.thrusters.right == BJ_TRUE ? "\x1B[32mYES\x1B[0m" : " \x1B[31mno\x1B[0m"
    );

    bj_draw_textf(target, 10, 10, size, white, "angle: \x1B[37m%.2lf\x1B[0m deg", data->lander.body.angular.value * 180.f / BJ_PI);
}

////////////////////////////////////////////////////////////////////////////////
/// Physics
static void apply_thrusters(bj_rigid_body_2d* p_body, lander* l) {
    if(l->thrusters.up) {
        bj_vec2 force;
        const bj_real angle = l->body.angular.value;

        bj_vec2_set(force, 
            bj_sin(-angle) * l->thrusters.magnitude,
            bj_cos(angle) * l->thrusters.magnitude
        );

        bj_apply_particle_force_2d(&p_body->particle, force);
    }

    const float torque = BJ_F(10.0);

    if(l->thrusters.left) {
        bj_apply_angular_torque_2d(&l->body.angular, torque);
    }
    if(l->thrusters.right) {
        bj_apply_angular_torque_2d(&l->body.angular, -torque);
    }
}

static void physics(game_data* data, double delta_time) {
    bj_apply_gravity_2d(&data->lander.body.particle, data->world.g);
    bj_apply_drag_2d(&data->lander.body.particle, data->lander.drag_k1, data->lander.drag_k2);
    apply_thrusters(&data->lander.body, &data->lander);

    bj_step_rigid_body_2d(&data->lander.body, delta_time);
}

///////////////////////////////////////////////////////////////////////////////
/// Program
int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)argc; (void)argv;

    bj_error* p_error = 0;
    if(!bj_initialize(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    game_data* data        = bj_calloc(sizeof(game_data));
    data->window           = bj_bind_window("Moonlander", 0,0, SCREEN_W, SCREEN_H, 0);
    data->draw.framebuffer = bj_get_window_framebuffer(data->window, 0);

    bj_vec2_set(data->lander.body.particle.position, BJ_F(0.0), BJ_F(40.0));
    data->lander.body.particle.inverse_mass   = BJ_FI(8.0);
    data->lander.body.particle.damping        = BJ_F(.995);
    data->lander.body.angular.inverse_inertia = BJ_FI(3.);
    data->lander.body.angular.damping         = BJ_F(.96);
    data->lander.thrusters.magnitude          = BJ_F(90.);
    data->world.g                             = BJ_F(12.);
    data->lander.drag_k1                      = BJ_F(0.490);
    data->lander.drag_k2                      = BJ_F(0.049);

    *user_data          = data;

    prepare_assets(data);
    update_projection(data);

    bj_set_key_callback(bj_close_on_escape, 0);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    game_data* data = (game_data*)user_data;
    const double dt = bj_step_delay_stopwatch(&data->stopwatch);

    bj_dispatch_events();

    data->lander.thrusters.up    = bj_get_key(data->window, BJ_KEY_UP);
    data->lander.thrusters.right = bj_get_key(data->window, BJ_KEY_RIGHT);
    data->lander.thrusters.left  = bj_get_key(data->window, BJ_KEY_LEFT);

    physics(data, dt);
    draw(data);
    bj_update_window_framebuffer(data->window);

    bj_sleep(15);

    return bj_should_close_window(data->window) 
         ? bj_callback_exit_success 
         : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    game_data* data = (game_data*)user_data;
    bj_unbind_window(data->window);
    bj_shutdown(0);
    return status;
}
