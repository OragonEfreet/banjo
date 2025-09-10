////////////////////////////////////////////////////////////////////////////////
/// \example physics_spring
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/mat.h>
#include <banjo/physics_2d.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/vec.h>
#include <banjo/window.h>

#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600

bj_window* window      = 0;
bj_bitmap* framebuffer = 0;
bj_mat3 projection;

#define BOXES_LEN 1
#define GRAVITY BJ_F(9.80)

typedef struct {
    bj_vec2 anchor;
    bj_real resting_length;
    bj_particle_2d particle;
    uint32_t color;
} box;

static BJ_INLINE void draw_spring(
    bj_bitmap*    p_bitmap,
    const bj_vec2 p0,
    const bj_vec2 p1,
    uint32_t      color
);

box boxes[BOXES_LEN];

bj_stopwatch stopwatch;

static void update_projection() {
    bj_mat3 ortho, viewport;
    bj_mat3_ortho(ortho, -CANVAS_WIDTH/2, CANVAS_WIDTH/2, -CANVAS_HEIGHT/2, CANVAS_HEIGHT/2);
    bj_mat3_viewport(viewport, 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT);
    bj_mat3_mul(projection, viewport, ortho);
    bj_info("Projection updated");
}

static void reset_box(size_t at) {
    const uint8_t r = (uint8_t)(128 + rand() % 128);
    const uint8_t g = (uint8_t)(128 + rand() % 128);
    const uint8_t b = (uint8_t)(128 + rand() % 128);

    box* abox = &boxes[at];

    switch (rand() % 3) {
        case 0: abox->color = bj_bitmap_pixel_value(framebuffer, 100, g, b); break;
        case 1: abox->color = bj_bitmap_pixel_value(framebuffer, r, 100, b); break;
        default: abox->color = bj_bitmap_pixel_value(framebuffer, r, g, 100); break;
    }

    bj_vec2_zero(abox->particle.position);
    bj_vec2_zero(abox->particle.velocity);
    bj_vec2_zero(abox->particle.acceleration);
    bj_vec2_set(abox->anchor, BJ_FZERO, BJ_F(100.0));
    abox->resting_length = BJ_F(100.0);
    abox->particle.inverse_mass = BJ_F(1.);
    abox->particle.damping = BJ_F(1.0);
}

static void initialize_boxes() {
    for(size_t b = 0 ; b < BOXES_LEN ; ++b) {
        reset_box(b);
    }
}

static void physics(bj_real dt) {
    for(size_t i = 0 ; i < BOXES_LEN ; ++i) {

        bj_accumulate_anchor_spring_2d(
            &boxes[i].particle, 
            boxes[i].anchor,
            BJ_F(70.0),
            BJ_F(10.0)
        );
        bj_integrate_particle_2d(&boxes[i].particle, dt);
    }
}

static void draw() {
    bj_bitmap_clear(framebuffer);
    size_t half_size = 5;
    const uint32_t spring_color = bj_bitmap_pixel_value(framebuffer, 0xAA, 0xAA, 0xAA);


    for(size_t b = 0 ; b < BOXES_LEN ; ++b) {
        const box* abox = boxes+b;

        // Box
        bj_vec3 box_top = {[2] = BJ_F(1.0)};
        bj_vec3 p_box_top;

        bj_vec2_copy(box_top, abox->particle.position);
        bj_mat3_mul_vec3(p_box_top, projection, box_top);

        bj_bitmap_draw_filled_rectangle(framebuffer,
            &(bj_rect){
                .x = p_box_top[0] - half_size,
                .y = p_box_top[1],
                .w = 2 * half_size,
                .h = 2 * half_size,
            },
            boxes[b].color
        );

        // Spring
        bj_vec3 anchor = {[2] = BJ_F(1.0)};
        bj_vec3 p_anchor;

        bj_vec2_copy(anchor, abox->anchor);
        bj_mat3_mul_vec3(p_anchor, projection, anchor);

        draw_spring(
            framebuffer,
            p_anchor,
            p_box_top,
            abox->color
        );

        bj_bitmap_draw_filled_circle(framebuffer,
            p_anchor[0] - half_size,
            p_anchor[1],
            BJ_F(5.0),
            spring_color
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

    window = bj_window_new("2D Spring", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    bj_set_key_callback(bj_close_on_escape, 0);

    framebuffer = bj_window_get_framebuffer(window, 0);

    initialize_boxes();
    update_projection();

    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();

    physics(bj_stopwatch_step_delay(&stopwatch));
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

static BJ_INLINE void draw_spring(
    bj_bitmap*    p_bitmap,
    const bj_vec2 p0,
    const bj_vec2 p1,
    uint32_t      color
) {
    int coils = 10;
    bj_real amplitude = 10.f;
    const bj_real x0 = p0[0], y0 = p0[1];
    const bj_real x1 = p1[0], y1 = p1[1];

    bj_real dx = x1 - x0, dy = y1 - y0;
    bj_real L  = bj_sqrt(dx*dx + dy*dy);
    if (bj_real_is_zero(L)) {
        bj_bitmap_draw_line(p_bitmap,(int)x0,(int)y0,(int)x0,(int)y0,color);
        return;
    }

    bj_real ux = dx / L, uy = dy / L;
    bj_real nx = -uy,   ny =  ux;

    if (coils < 2) coils = 2;

    bj_real px = x0, py = y0;
    for (int i = 1; i <= coils; ++i) {
        bj_real t  = (bj_real)i / (bj_real)coils;
        bj_real cx = x0 + ux * (t * L);
        bj_real cy = y0 + uy * (t * L);

        bj_real off = (i & 1) ? amplitude : -amplitude;
        bj_real qx  = cx + nx * off;
        bj_real qy  = cy + ny * off;

        bj_bitmap_draw_line(p_bitmap,
            (int)bj_round(px), (int)bj_round(py),
            (int)bj_round(qx), (int)bj_round(qy),
            color);

        px = qx; py = qy;
    }

    bj_bitmap_draw_line(p_bitmap,
        (int)bj_round(px), (int)bj_round(py),
        (int)bj_round(x1), (int)bj_round(y1),
        color);
}
