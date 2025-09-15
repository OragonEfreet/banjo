////////////////////////////////////////////////////////////////////////////////
/// \example drawing_2d.c
///  2D primitive drawing demo.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/pixel.h>
#include <banjo/random.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define WINDOW_W 800
#define WINDOW_H 600

#define BORDER_W 25
#define BORDER_H 15

#define GRAPH_W (WINDOW_W - 230)
#define GRAPH_H (WINDOW_H - BORDER_H * 2)

#define N_DISTRIBUTIONS 2

bj_pcg32 data = {
    .state    = 1,
    .inc      = 1,
};

typedef struct {
    const char*  name;
    size_t       result[GRAPH_W];
    uint32_t     color;
} distribution;

size_t n_steps = 65536;
size_t min_y = 0;
size_t max_y = 0;

distribution distributions[N_DISTRIBUTIONS];
bj_window* window = 0;
bj_bitmap* framebuffer = 0;

static void init_distributions(bj_bitmap* bmp) {
    distributions[0].name = "uniform_int32";
    distributions[0].color = bj_bitmap_pixel_value(bmp, 0x00, 0xFF, 0x00);

    distributions[1].name = "bernoulli";
    distributions[1].color = bj_bitmap_pixel_value(bmp, 0x00, 0x00, 0xFF);
}

#include <stdlib.h>

static void run_distributions() {
    min_y = 0;
    max_y = 0;

    for(size_t d = 0 ; d < N_DISTRIBUTIONS ; ++d) {
        bj_memzero(distributions[d].result, sizeof(size_t) * GRAPH_W); 
    }

    // Uniform distribution
    for(size_t s = 0 ; s < n_steps ; ++s) {
        uint32_t value = bj_uniform_int32_distribution(bj_pcg32_generator, &data, 0, GRAPH_W-1);
        size_t y = ++distributions[0].result[value];
        if (y > max_y) max_y = y;
        if (y < min_y) min_y = y;
    }

    // Bernoulli
    for(size_t px = 0 ; px < GRAPH_W ; ++px) {
        bj_real p = (bj_real)px / GRAPH_W;
        for(size_t s = 0 ; s < n_steps ; ++s) {
            if(bj_bernoulli_distribution(bj_pcg32_generator, &data, p)) {
                size_t y = ++distributions[1].result[px];
                if (y > max_y) max_y = y;
                if (y < min_y) min_y = y;
            }
        }
    }

    bj_info("distribution done");
}

static uint32_t darken_color(uint32_t pixel, double factor, bj_bitmap* bmp) {
    uint8_t r, g, b;
    bj_pixel_rgb(bj_bitmap_mode(bmp), pixel, &r, &g, &b); /* assume you have such utility */
    r = (uint8_t)(r * factor);
    g = (uint8_t)(g * factor);
    b = (uint8_t)(b * factor);
    return bj_bitmap_pixel_value(bmp, r, g, b);
}

void draw(bj_bitmap* bmp) {
    bj_bitmap_clear(bmp);

    const uint32_t color_box  = bj_bitmap_pixel_value(bmp, 0x30, 0x30, 0x30);
    const uint32_t color_tick = bj_bitmap_pixel_value(bmp, 0x40, 0x40, 0x40);
    const uint32_t white = bj_bitmap_pixel_value(bmp, 0xAA, 0xAA, 0xAA);

    bj_rect graph_box = (bj_rect){
        .x = WINDOW_W - BORDER_W - GRAPH_W,
        .y = BORDER_H,
        .w = GRAPH_W,
        .h = GRAPH_H
    };
    bj_bitmap_draw_rectangle(bmp, &graph_box, color_box);

    double scale = (max_y > min_y)
                 ? (double)(GRAPH_H - 1) / (double)(max_y - min_y)
                 : 0.0;

    /* y-axis ticks: one per count unit, drawn on the left border */
    if (scale > 0.0) {
        const int tick_len = 4; /* pixels inside the box */
        for (size_t v = min_y; v <= max_y; ++v) {
            int yscaled = (int)((v - min_y) * scale + 0.5);
            int sy = WINDOW_H - BORDER_H - 1 - yscaled;
            if (sy >= graph_box.y && sy < graph_box.y + graph_box.h) {
                bj_bitmap_draw_line(bmp,
                    graph_box.x,              sy,
                    graph_box.x + tick_len,   sy,
                    color_tick);
            }
        }
    }

    for (size_t d = 0; d < N_DISTRIBUTIONS; ++d) {
        uint32_t color_curve = distributions[d].color;
        uint32_t color_dots  = darken_color(color_curve, 0.7, bmp);

        /* dots */
        for (int x = 0; x < GRAPH_W; ++x) {
            size_t ycount = distributions[d].result[x];
            int yscaled = (int)((ycount - min_y) * scale + 0.5);
            int sx = graph_box.x + x;
            int sy = WINDOW_H - BORDER_H - 1 - yscaled;
            bj_bitmap_put_pixel(bmp, sx, sy, color_dots);
        }

        /* moving-average curve */
        const int W = 21;
        int prev_set = 0, px = 0, py = 0;
        for (int x = 0; x < GRAPH_W; ++x) {
            int half = W / 2;
            int xl = (x - half < 0) ? 0 : x - half;
            int xr = (x + half >= GRAPH_W) ? GRAPH_W - 1 : x + half;
            uint64_t sum = 0;
            for (int i = xl; i <= xr; ++i) sum += distributions[d].result[i];
            double avg = (double)sum / (double)(xr - xl + 1);

            int yscaled = (int)((avg - (double)min_y) * scale + 0.5);
            int sx = graph_box.x + x;
            int sy = WINDOW_H - BORDER_H - 1 - yscaled;

            if (prev_set) bj_bitmap_draw_line(bmp, px, py, sx, sy, color_curve);
            px = sx; py = sy; prev_set = 1;
        }

        /* legend */
        int lx = BORDER_W, ly = 10 + 15 * d;
        bj_bitmap_draw_filled_rectangle(
            bmp, &(bj_rect){ .x = lx, .y = ly, .w = 25, .h = 8 }, distributions[d].color);
        bj_bitmap_printf(bmp, lx + 30, ly, 8, white, distributions[d].name);
    }

    
    bj_bitmap_printf(bmp, 30, WINDOW_H - 30 - 20, 8, white, "%ld draws", n_steps);
    bj_bitmap_printf(bmp, 30, WINDOW_H - 30 - 10, 8, white, " Left/Right arrow");
    bj_bitmap_printf(bmp, 30, WINDOW_H - 30, 8, white, " keys to change");
}

static void roll() {
    run_distributions();
    draw(framebuffer);
    bj_window_update_framebuffer(window);
}

void key_callback(bj_window* p_window, const bj_key_event* e, void* data) {
    (void)data;

    if(e->action != BJ_RELEASE) {
        return;
    }

    switch(e->key) {
        case BJ_KEY_RETURN:
            roll();
            break;
        case BJ_KEY_LEFT:
            if(n_steps > 10) {
                n_steps /= 2;
                bj_info("%ld", n_steps);
                roll();
            }
            break;
        case BJ_KEY_RIGHT:
            if(n_steps < 0x20000000) {
                n_steps *= 2;
                bj_info("%ld", n_steps);
                roll();
            }
            break;
        case BJ_KEY_ESCAPE:
            bj_window_set_should_close(p_window);
            break;
        default:
            break;
    }


}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    bj_error* p_error = 0;

    if(!bj_begin(&p_error)) {
        bj_err("Error 0x%08X: %s", p_error->code, p_error->message);
        return bj_callback_exit_error;
    } 

    window = bj_window_new("Random Distribution", 100, 100, WINDOW_W, WINDOW_H, 0);
    bj_set_key_callback(key_callback, 0);


    framebuffer = bj_window_get_framebuffer(window, 0);

    init_distributions(framebuffer);
    roll();


    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_sleep(30);

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

