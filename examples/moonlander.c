#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/geometry_2d.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/mat.h>
#include <banjo/math.h>
#include <banjo/physics.h>
#include <banjo/physics_2d.h>
#include <banjo/random.h>
#include <banjo/renderer.h>
#include <banjo/shader.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#include <math.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define CANVAS_W 800
#define CANVAS_H 600

#define BOX_W (CANVAS_W / 4)
#define BOX_H (CANVAS_H / 4)

#define LANDER_VERTICES_LEN 22
#define LANDER_EDGES_LEN 25

#define TERRAIN_MIN_X (-CANVAS_W)
#define TERRAIN_MAX_X (CANVAS_W)
#define TERRAIN_HEIGHTS_LEN 100
#define TERRAIN_BASE_HEIGHT (-CANVAS_H * BJ_F(0.5) + BJ_F(50.0))
#define TERRAIN_FLAT_TOLERANCE BJ_F(0.3)
#define TERRAIN_ANGLE_TOLERANCE BJ_F(0.1)

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

    bj_real bounding_radius;
} lander;

typedef struct {
    bj_real radius;
    bj_real angle;
} polar_coords;

#define CONTROL 0x01
#define PHYSICS 0x02
#define COLLIDE 0x04
#define GAME_PLAY (CONTROL | PHYSICS | COLLIDE)
#define EXPLODE 0x08

typedef struct {
    bj_window* window;
    bj_renderer* renderer;

    int state_set;
    bj_stopwatch state_since;

    struct {
        bj_real g;
    } world;

    lander lander;

    struct {
        bj_bitmap* framebuffer;
        polar_coords coords[LANDER_VERTICES_LEN];
        size_t edges[LANDER_EDGES_LEN][2];
        size_t fire_attache_edges[2];
        bj_mat3x3 projection;
    } draw;

    struct {
        bj_real heights[TERRAIN_HEIGHTS_LEN];
    } terrain;

} game_data;

static void set_state(game_data* data, int flags) {
    data->state_set = flags;
    bj_reset_stopwatch(&data->state_since);
}

static bj_bool check_state(const game_data* data, int flags) {
    return (data->state_set & flags) != 0;
}

static void prepare_data(game_data* data) {
    data->lander.body.particle.position = (bj_vec2){BJ_F(20.0), BJ_F(-40.0)};
    data->lander.body.particle.inverse_mass = BJ_FI(8.0);
    data->lander.body.particle.damping = BJ_F(.995);
    data->lander.body.angular.inverse_inertia = BJ_FI(3.);
    data->lander.body.angular.damping = BJ_F(.5);
    data->lander.thrusters.magnitude = BJ_F(90.);
    data->world.g = BJ_F(50.);
    data->lander.drag_k1 = BJ_F(0.490);
    data->lander.drag_k2 = BJ_F(0.049);
    set_state(data, GAME_PLAY);
}

///////////////////////////////////////////////////////////////////////////////
/// Drawing
static void prepare_assets(game_data* data) {

    const bj_real lander_coords_m[][2] = {
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
    bj_real min_y = lander_coords_m[0][1];
    bj_real max_y = lander_coords_m[0][1];
    for (size_t c = 1; c < LANDER_VERTICES_LEN; ++c) {
        const bj_real y = lander_coords_m[c][1];
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }
    const bj_real full_height = max_y - min_y;
    const bj_real size_ratio = BJ_F(7.) / full_height;

    data->lander.bounding_radius = BJ_FZERO;
    for (size_t c = 0; c < LANDER_VERTICES_LEN; ++c) {
        const bj_real x = lander_coords_m[c][0];
        const bj_real y = lander_coords_m[c][1];
        const bj_real radius = bj_sqrt(x * x + y * y) * size_ratio;
        data->draw.coords[c].radius = radius;
        data->draw.coords[c].angle = bj_atan2(y, x);
        if (radius > data->lander.bounding_radius) {
            data->lander.bounding_radius = radius;
        }
    }

    data->draw.fire_attache_edges[0] = 15;
    data->draw.fire_attache_edges[1] = 16;

    bj_memcpy(data->draw.edges, lander_edges, sizeof(lander_edges));

    // Terrain
    const bj_real mean = TERRAIN_BASE_HEIGHT;
    const bj_real sd = BJ_F(10.);
    for (size_t h = 0; h < TERRAIN_HEIGHTS_LEN; ++h) {
        data->terrain.heights[h] = bj_normal_real_distribution(bj_pcg32_generator, 0, mean, sd);
    }

}

static void update_projection(game_data* data) {
    bj_mat3x3 ortho;
    bj_mat3x3 viewport;

    if (data->lander.body.particle.position.y - BOX_H <= -CANVAS_H * BJ_F(0.5)) {

        const int iy = (int)floor((double)(data->lander.body.particle.position.y / BOX_H));
        const int ix = (int)floor((double)(data->lander.body.particle.position.x / BOX_W));

        const bj_real lx = (bj_real)ix * BOX_W + BOX_W * BJ_F(0.5);
        const bj_real ly = (bj_real)iy * BOX_H + BOX_H * BJ_F(0.5);

        bj_mat3_set_ortho(&ortho,
            lx - BOX_W * BJ_F(0.5),
            lx + BOX_W * BJ_F(0.5),
            ly - BOX_H * BJ_F(0.5),
            ly + BOX_H * BJ_F(0.5)
        );
    }
    else {
        bj_mat3_set_ortho(&ortho, -CANVAS_W * BJ_F(0.5), CANVAS_W * BJ_F(0.5), -CANVAS_H * BJ_F(0.5), CANVAS_H * BJ_F(0.5));
    }

    bj_mat3_set_viewport(&viewport, BJ_FZERO, BJ_FZERO, SCREEN_W, SCREEN_H);
    bj_mat3_mul(&data->draw.projection, &viewport, &ortho);
}


static BJ_INLINE bj_real bj_wrap_pi(bj_real a) {
    // wrap to (-pi, pi]
    while (a <= -BJ_PI) a += BJ_TAU;   // BJ_TAU = 2*pi
    while (a > BJ_PI) a -= BJ_TAU;
    return a;
}

static bj_bool bj_angle_match(bj_real theta_ship,
    bj_real theta_floor,
    bj_real max_diff)         // e.g. 0.10f
{
    bj_real d = bj_wrap_pi(theta_ship - theta_floor);
    return bj_abs(d) <= max_diff;
}

static bj_bool floor_is_flat(bj_real theta, bj_real max_alpha) {
    bj_real a = bj_abs(theta);
    if (a > BJ_PI * BJ_F(0.5)) a = BJ_PI - a;  // fold into [0, pi/2]
    return a <= max_alpha;                      // e.g. max_alpha=0.0873 or 0.1745
}

static void draw(game_data* data) {
    bj_bitmap* target = data->draw.framebuffer;

    const uint32_t color_a = bj_make_bitmap_pixel(target, 0x00, 0xCC, 0x44);
    const uint32_t color_b = bj_make_bitmap_pixel(target, 0xCC, 0x44, 0x00);
    const uint32_t white = bj_make_bitmap_pixel(target, 0xAA, 0xAA, 0xAA);
    bj_clear_bitmap(target);

    bj_vec3 p0, q0;
    bj_vec3 p1, q1;

    const bj_real x = data->lander.body.particle.position.x;
    const bj_real y = data->lander.body.particle.position.y;


    for (size_t e = 0; e < LANDER_EDGES_LEN; ++e) {
        const bj_real r0 = data->draw.coords[data->draw.edges[e][0]].radius;
        const bj_real a0 = data->draw.coords[data->draw.edges[e][0]].angle + data->lander.body.angular.value;
        const bj_real r1 = data->draw.coords[data->draw.edges[e][1]].radius;
        const bj_real a1 = data->draw.coords[data->draw.edges[e][1]].angle + data->lander.body.angular.value;

        const bj_real x0 = bj_cos(a0) * r0;
        const bj_real y0 = bj_sin(a0) * r0;
        const bj_real x1 = bj_cos(a1) * r1;
        const bj_real y1 = bj_sin(a1) * r1;


        if (check_state(data, EXPLODE)) {

            // Compute the midpoint and use it as the initial velocity
            const bj_vec2 midpoint = {
                (x0 + x1) * BJ_F(0.5), (y0 + y1) * BJ_F(0.5)
            };
            const bj_vec2 initial_position = {
                midpoint.x + x,
                midpoint.y + y,
            };
            const bj_vec2 initial_velocity = {
                midpoint.x * BJ_F(15.) + data->lander.body.particle.velocity.x * BJ_F(1.5),
                bj_abs(midpoint.y * BJ_F(15.)),
            };
            const bj_vec2 acceleration = { BJ_FZERO, -data->world.g };
            bj_vec2 res = { BJ_FZERO, BJ_FZERO };
            res = bj_compute_kinematics_2d(
                initial_position,
                initial_velocity,
                acceleration,
                bj_stopwatch_elapsed(&data->state_since)
            );

            const bj_vec2 displacement = bj_vec2_sub(res, initial_position);

            q0 = (bj_vec3){x0 + displacement.x + x, y0 + displacement.y + y, BJ_F(1.0)};
            q1 = (bj_vec3){x1 + displacement.x + x, y1 + displacement.y + y, BJ_F(1.0)};

        }
        else {
            q0 = (bj_vec3){x0 + x, y0 + y, BJ_F(1.0)};
            q1 = (bj_vec3){x1 + x, y1 + y, BJ_F(1.0)};
        }


        p0 = bj_mat3_transform_vec3(&data->draw.projection, q0);
        p1 = bj_mat3_transform_vec3(&data->draw.projection, q1);

        bj_draw_line(
            target,
            p0.x, p0.y,
            p1.x, p1.y,
            check_state(data, EXPLODE) && (((int)(p0.x + p0.y) % 2)) ? color_b : color_a
        );

    }

    // Thrust fire
    const bj_real min_radius = BJ_F(5.);
    static size_t inc_radius = 0;

    if (data->lander.thrusters.up) {
        if (inc_radius <= 10) {
            ++inc_radius;
        }
    }
    else {
        if (inc_radius > 0) {
            --inc_radius;
        }
    }

    if (inc_radius > 0) {
        bj_real elapsed = (bj_real)bj_stopwatch_elapsed(&data->state_since);

        const bj_real add_radius = ((bj_real)inc_radius) * BJ_F(1.5);

        for (size_t i = 0; i < 4; ++i) {

            bj_real variance = bj_sin(elapsed * BJ_F(40.)) * BJ_F(1.5);
            bj_real radius = min_radius + add_radius + variance;

            if (i > 0) {
                radius = min_radius + bj_fmod(elapsed * ((bj_real)i * BJ_F(13.)), add_radius);
            }

            polar_coords fire_coords[] = {
                {data->draw.coords[data->draw.fire_attache_edges[0]].radius, data->draw.coords[data->draw.fire_attache_edges[0]].angle, },
                {radius, -BJ_PI / BJ_F(2.0)},
                {data->draw.coords[data->draw.fire_attache_edges[1]].radius, data->draw.coords[data->draw.fire_attache_edges[1]].angle, },
            };

            int fire_x[3];
            int fire_y[3];

            for (size_t c = 0; c < 3; ++c) {
                q0 = (bj_vec3){
                    bj_cos(fire_coords[c].angle + data->lander.body.angular.value) * fire_coords[c].radius + x,
                    bj_sin(fire_coords[c].angle + data->lander.body.angular.value) * fire_coords[c].radius + y,
                    BJ_F(1.),
                };

                p0 = bj_mat3_transform_vec3(&data->draw.projection, q0);
                fire_x[c] = p0.x;
                fire_y[c] = p0.y;
            }


            bj_draw_polyline(target, 3, fire_x, fire_y, BJ_FALSE, color_b);
        }
    }


    // Terrain
#if TERRAIN_HEIGHTS_LEN > 1
    const bj_real terrain_length = TERRAIN_MAX_X - TERRAIN_MIN_X;
    for (size_t h = 0; h < TERRAIN_HEIGHTS_LEN - 1; ++h) {
        q0 = (bj_vec3){
            TERRAIN_MIN_X + ((bj_real)h) / ((bj_real)TERRAIN_HEIGHTS_LEN - 1) * terrain_length,
            data->terrain.heights[h],
            BJ_F(1.),
        };
        q1 = (bj_vec3){
            TERRAIN_MIN_X + ((bj_real)h + 1) / ((bj_real)TERRAIN_HEIGHTS_LEN - 1) * terrain_length,
            data->terrain.heights[h + 1],
            BJ_F(1.),
        };

        p0 = bj_mat3_transform_vec3(&data->draw.projection, q0);
        p1 = bj_mat3_transform_vec3(&data->draw.projection, q1);

        const bj_real floor_angle = bj_atan2(
            p1.y - p0.y, p1.x - p0.x
        );
        const uint32_t c = floor_is_flat(floor_angle, TERRAIN_FLAT_TOLERANCE) ? color_a : color_b;

        bj_draw_line(target, p0.x, p0.y, p1.x, p1.y, c);



    }
#endif

    // UI text
    const unsigned size = 10;
    bj_draw_textf(target, 10, SCREEN_H - 10 - size, size, white, "states: %s, %s, %s, %s",
        check_state(data, CONTROL) == BJ_TRUE ? "\x1B[32mcontrol\x1B[0m" : "\x1B[31mcontrol\x1B[0m",
        check_state(data, PHYSICS) == BJ_TRUE ? "\x1B[32mphysics\x1B[0m" : "\x1B[31mphysics\x1B[0m",
        check_state(data, COLLIDE) == BJ_TRUE ? "\x1B[32mcollide\x1B[0m" : "\x1B[31mcollide\x1B[0m",
        check_state(data, EXPLODE) == BJ_TRUE ? "\x1B[32mexplode\x1B[0m" : "\x1B[31mexplode\x1B[0m"
    );

    /* bj_draw_textf(target, 10, 10, size, white, buf); */

}


////////////////////////////////////////////////////////////////////////////////
/// Physics

static void apply_thrusters(bj_rigid_body_2d* p_body, lander* l) {
    if (l->thrusters.up) {
        const bj_real angle = l->body.angular.value;

        const bj_vec2 force = {
            .x = bj_sin(-angle) * l->thrusters.magnitude,
            .y = bj_cos(angle) * l->thrusters.magnitude,
        };

        bj_apply_particle_force_2d(&p_body->particle, force);
    }

    const bj_real torque = BJ_F(10.0);

    if (l->thrusters.left) {
        bj_apply_angular_torque_2d(&l->body.angular, torque);
    }
    if (l->thrusters.right) {
        bj_apply_angular_torque_2d(&l->body.angular, -torque);
    }
}


static void handle_collision(game_data* data) {
#if TERRAIN_HEIGHTS_LEN > 1
    bj_vec2 l0, l1;

    const bj_vec2 c = data->lander.body.particle.position;
    const bj_real r = data->lander.bounding_radius;

    const bj_real terrain_length = TERRAIN_MAX_X - TERRAIN_MIN_X;
    size_t h = 0;
    for (; h < TERRAIN_HEIGHTS_LEN - 1; ++h) {

        l0.x = TERRAIN_MIN_X + ((bj_real)h) / ((bj_real)TERRAIN_HEIGHTS_LEN - 1) * terrain_length;
        l0.y = data->terrain.heights[h];

        l1.x = TERRAIN_MIN_X + ((bj_real)h + 1) / ((bj_real)TERRAIN_HEIGHTS_LEN - 1) * terrain_length;
        l1.y = data->terrain.heights[h + 1];

        if (bj_check_circle_segment_hit(c, r, l0, l1)) {
            break;
        }
    }

    // First pass collision detected
    if (h < TERRAIN_HEIGHTS_LEN - 1) {

        for (size_t e = 0; e < LANDER_EDGES_LEN; ++e) {
            const bj_real r0 = data->draw.coords[data->draw.edges[e][0]].radius;
            const bj_real a0 = data->draw.coords[data->draw.edges[e][0]].angle + data->lander.body.angular.value;
            const bj_real r1 = data->draw.coords[data->draw.edges[e][1]].radius;
            const bj_real a1 = data->draw.coords[data->draw.edges[e][1]].angle + data->lander.body.angular.value;

            const bj_vec2 s0 = {
                data->lander.body.particle.position.x + bj_cos(a0) * r0,
                data->lander.body.particle.position.y + bj_sin(a0) * r0,
            };

            const bj_vec2 s1 = {
                data->lander.body.particle.position.x + bj_cos(a1) * r1,
                data->lander.body.particle.position.y + bj_sin(a1) * r1,
            };

            if (bj_check_segments_hit(s0, s1, l0, l1)) {

                const bj_real floor_angle = bj_atan2(l1.y - l0.y, l1.x - l0.x);

                const bj_bool landing = floor_is_flat(floor_angle, TERRAIN_FLAT_TOLERANCE)
                    & bj_angle_match(data->lander.body.angular.value, floor_angle, TERRAIN_ANGLE_TOLERANCE);

                if (!landing) set_state(data, EXPLODE); else set_state(data, 0);
                break;
            }
        }
    }
#endif

}

static void physics(game_data* data, double delta_time) {
    if (check_state(data, COLLIDE)) {
        handle_collision(data);
    }
    if (check_state(data, PHYSICS)) {
        bj_apply_gravity_2d(&data->lander.body.particle, data->world.g);
        bj_apply_drag_2d(&data->lander.body.particle, data->lander.drag_k1, data->lander.drag_k2);
        apply_thrusters(&data->lander.body, &data->lander);
        bj_step_rigid_body_2d(&data->lander.body, delta_time);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Program
int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)argc; (void)argv;

    bj_error* p_error = 0;
    if (!bj_begin(BJ_VIDEO_SYSTEM, &p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    }

    game_data* data = bj_calloc(sizeof(game_data));
    data->window = bj_bind_window("Moonlander", 0, 0, SCREEN_W, SCREEN_H, 0);

    *user_data = data;

    prepare_data(data);
    prepare_assets(data);

    data->renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE);
    bj_renderer_configure(data->renderer, data->window);
    data->draw.framebuffer = bj_get_framebuffer(data->renderer);

    bj_set_key_callback(bj_close_on_escape, 0);

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    game_data* data = (game_data*)user_data;
    const double dt = bj_step_delay_stopwatch(&data->state_since);

    bj_dispatch_events();

    if (check_state(data, CONTROL)) {
        data->lander.thrusters.up = bj_get_key(data->window, BJ_KEY_UP);
        data->lander.thrusters.right = bj_get_key(data->window, BJ_KEY_RIGHT);
        data->lander.thrusters.left = bj_get_key(data->window, BJ_KEY_LEFT);
    }
    else {
        data->lander.thrusters.up = BJ_FALSE;
        data->lander.thrusters.right = BJ_FALSE;
        data->lander.thrusters.left = BJ_FALSE;
    }


    physics(data, dt);
    update_projection(data);
    draw(data);
    bj_present(data->renderer, data->window);

    bj_sleep(15);

    return bj_should_close_window(data->window)
        ? bj_callback_exit_success
        : bj_callback_continue;
}

int bj_app_end(void* user_data, int status) {
    game_data* data = (game_data*)user_data;
    bj_destroy_renderer(data->renderer);
    bj_unbind_window(data->window);
    bj_end();
    return status;
}
