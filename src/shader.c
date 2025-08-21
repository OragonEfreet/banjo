#include <banjo/shader.h>
#include "bitmap_t.h"
#include "check.h"

void bj_bitmap_apply_shader(
    bj_bitmap*               p_bitmap,
    bj_bitmap_shading_fn_t   p_shader,
    void*                    p_data,
    uint8_t                  flags
) {
    bj_check(p_bitmap);
    bj_check(p_shader);

    const int invert_x         = (flags & BJ_SHADER_INVERT_X) != 0;
    const int invert_y         = (flags & BJ_SHADER_INVERT_Y) != 0;
    const int normalize_coords = (flags & BJ_SHADER_NORMALIZE_COORDS) != 0;
    const int center_coords    = (flags & BJ_SHADER_CENTER_COORDS) != 0;
    const int clamp_color      = (flags & BJ_SHADER_CLAMP_COLOR) != 0;

    const size_t W = p_bitmap->width;
    const size_t H = p_bitmap->height;

    const bj_real fw = (bj_real)W;
    const bj_real fh = (bj_real)H;

    bj_real ax = BJ_F(1.0), bx = BJ_F(0.0);
    bj_real ay = BJ_F(1.0), by = BJ_F(0.0);

    if (invert_x) { ax = -ax; bx = fw - bx; }
    if (invert_y) { ay = -ay; by = fh - by; }

    if (normalize_coords) {
        const bj_real inv_w = (fw != BJ_F(0.0)) ? BJ_F(1.0) / fw : BJ_F(0.0);
        const bj_real inv_h = (fh != BJ_F(0.0)) ? BJ_F(1.0) / fh : BJ_F(0.0);
        ax *= inv_w; bx *= inv_w;
        ay *= inv_h; by *= inv_h;
    }

    if (center_coords) {
        const bj_real sx = normalize_coords ? BJ_F(0.5) : (fw * BJ_F(0.5));
        const bj_real sy = normalize_coords ? BJ_F(0.5) : (fh * BJ_F(0.5));
        const bj_real kx = normalize_coords ? BJ_F(2.0) : (fw * BJ_F(2.0));
        const bj_real ky = normalize_coords ? BJ_F(2.0) : (fh * BJ_F(2.0));
        bx = (kx * bx) - (kx * sx); ax *= kx;
        by = (ky * by) - (ky * sy); ay *= ky;
    }

    const bj_real inv255 = BJ_F(1.0) / BJ_F(255.0);

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {
            uint8_t r, g, b;
            bj_bitmap_rgb(p_bitmap, x, y, &r, &g, &b);

            bj_vec3 color = { (bj_real)r * inv255, (bj_real)g * inv255, (bj_real)b * inv255 };
            bj_vec2 frag_coords = { ax * (bj_real)x + bx, ay * (bj_real)y + by };

            if (p_shader(color, frag_coords, p_data) > 0) {
                if (clamp_color) {
                    color[0] = bj_clamp(color[0], BJ_F(0.0), BJ_F(1.0));
                    color[1] = bj_clamp(color[1], BJ_F(0.0), BJ_F(1.0));
                    color[2] = bj_clamp(color[2], BJ_F(0.0), BJ_F(1.0));
                }

                r = (uint8_t)(color[0] * BJ_F(255.0) + BJ_F(0.5));
                g = (uint8_t)(color[1] * BJ_F(255.0) + BJ_F(0.5));
                b = (uint8_t)(color[2] * BJ_F(255.0) + BJ_F(0.5));

                bj_bitmap_put_pixel(p_bitmap, x, y, bj_bitmap_pixel_value(p_bitmap, r, g, b));
            }
        }
    }
}
