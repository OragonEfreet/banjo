////////////////////////////////////////////////////////////////////////////////
/// \example random_distribution.c
/// Statistical random distributions with interactive histograms.
///
/// Random distributions generate numbers following specific probability patterns.
/// This example demonstrates three common distributions:
/// - Uniform: All values in a range are equally likely (dice rolls, card shuffles)
/// - Bernoulli: Binary true/false outcomes with probability p (coin flips, hit chances)
/// - Normal (Gaussian): Bell curve around a mean (measurement errors, natural variation)
///
/// The example visualizes each distribution as a histogram, showing how random
/// samples converge to the expected statistical shape as sample count increases.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUTOMAIN_CALLBACKS
#include <banjo/bitmap.h>
#include <banjo/draw.h>
#include <banjo/event.h>
#include <banjo/log.h>
#include <banjo/main.h>
#include <banjo/pixel.h>
#include <banjo/random.h>
#include <banjo/renderer.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/window.h>

#define WINDOW_W 800
#define WINDOW_H 600

#define BORDER_W 25
#define BORDER_H 15

#define GRAPH_W (WINDOW_W - BORDER_W * 2)
#define GRAPH_H (WINDOW_H - 100)

#define N_DISTRIBUTIONS 3

typedef struct {
    const char*  name;
    size_t       result[GRAPH_W];
    uint32_t     color;
    size_t       min_y;   /* fixed to 0 for histograms */
    size_t       max_y;   /* per-distribution */
    size_t       n_steps;
} distribution;

size_t n_steps_base = 524288;

distribution distributions[N_DISTRIBUTIONS];
bj_window* window      = 0;
bj_renderer* renderer  = 0;
bj_bitmap* framebuffer = 0;

static void init_distributions(bj_bitmap* bmp) {
    distributions[0].name    = "uniform:   %ld draws in [0;Xmax[ ; y = how many x";
    distributions[0].color   = bj_make_bitmap_pixel(bmp, 110, 231, 183);
    distributions[0].min_y   = 0;
    distributions[0].max_y   = 0;

    distributions[1].name  = "bernoulli: %ld draws with a probability p (x) ; y = how many hits";
    distributions[1].color = bj_make_bitmap_pixel(bmp, 147, 197, 253);
    distributions[1].min_y = 0;
    distributions[1].max_y = 0;

    distributions[2].name  = "normal:    %ld draws with Xmax/2 (mean) and 100 (deviation) ; y = how many x";
    distributions[2].color = bj_make_bitmap_pixel(bmp, 196, 181, 253);
    distributions[2].min_y = 0;
    distributions[2].max_y = 0;
}

static void run_distributions() {

    for (size_t d = 0; d < N_DISTRIBUTIONS; ++d) {
        bj_memzero(distributions[d].result, sizeof(size_t) * GRAPH_W);
        distributions[d].min_y = 0;
        distributions[d].max_y = 0;
    }

    distributions[0].n_steps = n_steps_base;
    distributions[1].n_steps = n_steps_base / 128;
    distributions[2].n_steps = n_steps_base / 64;

    // Uniform distribution: generates integers uniformly across [min, max].
    // bj_uniform_int32_distribution(generator, gen_data, min, max)
    // Every value in the range has equal probability. Use this for:
    // - Random array indices, positions, rotations
    // - Shuffling, dealing cards, rolling dice
    for (size_t s = 0; s < distributions[0].n_steps; ++s) {
        uint32_t x = bj_uniform_int32_distribution(bj_pcg32_generator, 0, 0, GRAPH_W - 1);
        size_t y = ++distributions[0].result[x];
        if (y > distributions[0].max_y) distributions[0].max_y = y;
    }

    // Bernoulli distribution: generates true/false with probability p.
    // bj_bernoulli_distribution(generator, gen_data, p)
    // Returns 1 with probability p, 0 with probability (1-p). Use this for:
    // - Coin flips, critical hit chances, spawn decisions
    // - Any yes/no outcome with a specific probability
    // Here we vary p across x to show the linear relationship.
    for (size_t px = 0; px < GRAPH_W; ++px) {
        bj_real p = (bj_real)px / GRAPH_W;
        for (size_t s = 0; s < distributions[1].n_steps; ++s) {
            if (bj_bernoulli_distribution(bj_pcg32_generator, 0, p)) {
                size_t y = ++distributions[1].result[px];
                if (y > distributions[1].max_y) distributions[1].max_y = y;
            }
        }
    }

    // Normal (Gaussian) distribution: generates values in a bell curve.
    // bj_normal_real_distribution(generator, gen_data, mean, std_deviation)
    // Most values cluster near the mean, with probability decreasing away from it.
    // Use this for:
    // - Natural variation (damage ranges, spawn positions with clustering)
    // - Measurement errors, particle velocities
    // - Anything that should be "usually X but sometimes a bit more/less"
    const bj_real mean = BJ_F(0.5) * (bj_real)(GRAPH_W - 1);
    const bj_real sd   = BJ_F(100.);
    for (size_t s = 0; s < distributions[2].n_steps; ++s) {
        bj_real v = bj_normal_real_distribution(bj_pcg32_generator, 0, mean, sd);
        long x = (long)bj_round(v);
        if (x < 0 || x >= (long)GRAPH_W) continue;           /* avoid OOB */
        size_t y = ++distributions[2].result[(size_t)x];
        if (y > distributions[2].max_y) distributions[2].max_y = y;
    }
}

static uint32_t darken_color(uint32_t pixel, double factor, bj_bitmap* bmp) {
    uint8_t r, g, b;
    bj_make_pixel_rgb(bj_bitmap_mode(bmp), pixel, &r, &g, &b);
    r = (uint8_t)(r * factor);
    g = (uint8_t)(g * factor);
    b = (uint8_t)(b * factor);
    return bj_make_bitmap_pixel(bmp, r, g, b);
}

// Visualize distributions as histograms. For each distribution, the x-axis
// represents value buckets and y-axis shows frequency (how many samples fell
// in that bucket). Dots show raw data, smooth curves show moving averages.
// This visualization helps understand how distributions behave statistically.
void draw(bj_bitmap* bmp) {
    bj_clear_bitmap(bmp);

    const uint32_t color_box  = bj_make_bitmap_pixel(bmp, 64, 72, 84);
    const uint32_t white      = bj_make_bitmap_pixel(bmp, 224, 230, 238);

    bj_rect graph_box = (bj_rect){
        .x = BORDER_W,
        .y = WINDOW_H - BORDER_H - GRAPH_H,
        .w = GRAPH_W,
        .h = GRAPH_H
    };
    bj_draw_rectangle(bmp, &graph_box, color_box);

    for (size_t d = 0; d < N_DISTRIBUTIONS; ++d) {
        size_t min_y = distributions[d].min_y; /* 0 for histograms */
        size_t max_y = distributions[d].max_y;
        double scale = (max_y > min_y)
                     ? (double)(GRAPH_H - 1) / (double)(max_y - min_y)
                     : 0.0;

        uint32_t color_curve = distributions[d].color;
        uint32_t color_dots  = darken_color(color_curve, 0.7, bmp);

        /* dots */
        for (int x = 0; x < GRAPH_W; ++x) {
            size_t ycount = distributions[d].result[x];
            int yscaled = (int)((ycount - min_y) * scale + 0.5);
            if (yscaled < 0) yscaled = 0;
            if (yscaled > GRAPH_H - 1) yscaled = GRAPH_H - 1;
            int sx = graph_box.x + x;
            int sy = WINDOW_H - BORDER_H - 1 - yscaled;
            bj_put_pixel(bmp, sx, sy, color_dots);
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
            if (yscaled < 0) yscaled = 0;
            if (yscaled > GRAPH_H - 1) yscaled = GRAPH_H - 1;
            int sx = graph_box.x + x;
            int sy = WINDOW_H - BORDER_H - 1 - yscaled;

            if (prev_set) bj_draw_line(bmp, px, py, sx, sy, color_curve);
            px = sx; py = sy; prev_set = 1;
        }

        /* legend */
        int lx = BORDER_W, ly = 10 + 15 * (int)d;
        bj_draw_filled_rectangle(
            bmp, &(bj_rect){ .x = lx, .y = ly, .w = 25, .h = 8 }, distributions[d].color);
        bj_draw_textf(bmp, lx + 30, ly, 8, white, distributions[d].name, distributions[d].n_steps);
    }
    bj_draw_textf(bmp, BORDER_W, 10 + 15 * (N_DISTRIBUTIONS + 1), 8, white, "Use Left/Right arrow keys to change number of draws.");

}

static void roll() {
    run_distributions();
    draw(framebuffer);
    bj_present(renderer, window);
}

// Interactive controls: adjust sample count to see how distributions converge.
// With few samples, histograms are noisy. With many samples, they approach
// the theoretical probability curves. This demonstrates the law of large numbers.
void key_callback(bj_window* p_window, const bj_key_event* e, void* data) {
    (void)data;
    if (e->action != BJ_RELEASE) return;

    switch (e->key) {
        case BJ_KEY_RETURN:
            roll(); break;
        case BJ_KEY_LEFT:
            if (n_steps_base > 10) { n_steps_base /= 2; roll(); }
            break;
        case BJ_KEY_RIGHT:
            if (n_steps_base < 0x20000000) { n_steps_base *= 2; roll(); }
            break;
        case BJ_KEY_ESCAPE:
            bj_set_window_should_close(p_window);
            break;
        default: break;
    }
}

int bj_app_begin(void** user_data, int argc, char* argv[]) {
    (void)user_data; (void)argc; (void)argv;

    if (!bj_begin(BJ_VIDEO_SYSTEM, 0)) {
        return bj_callback_exit_error;
    }

    renderer = bj_create_renderer(BJ_RENDERER_TYPE_SOFTWARE, 0);
    window = bj_bind_window("Random Distribution", 100, 100, WINDOW_W, WINDOW_H, 0, 0);

    bj_renderer_configure(renderer, window, 0);
    bj_set_key_callback(key_callback, 0);

    framebuffer = bj_get_framebuffer(renderer);

    bj_set_bitmap_color(framebuffer, bj_make_bitmap_pixel(framebuffer, 22, 26, 32), BJ_BITMAP_CLEAR_COLOR);

    init_distributions(framebuffer);
    roll();
    return bj_callback_continue;
}

int bj_app_iterate(void* user_data) {
    (void)user_data;
    bj_dispatch_events();
    bj_sleep(30);
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
