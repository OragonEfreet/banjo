#include <banjo/math.h>

#include "bitmap_t.h"
#include "check.h"


static int bj_bitmap_clipped_blit(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    bj_rect* p_dest_rect
) {
    // This is the "stupid blit". Just to test clipping works
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    
    bj_check_or_0(p_src);
    bj_check_or_0(p_dest);
    bj_check_or_0(p_src_rect->w == p_dest_rect->w);
    bj_check_or_0(p_src_rect->h == p_dest_rect->h);

    for(size_t r = 0 ; r < p_dest_rect->h ; ++r) {

        const size_t from_y = p_src_rect->y + r;
        const size_t to_y = p_dest_rect->y + r;
        for(size_t c = 0 ; c < p_dest_rect->w ; ++c) {
            bj_bitmap_rgb(p_src, p_src_rect->x + c, from_y, &red, &green, &blue);
            bj_bitmap_put_pixel(p_dest,
                p_dest_rect->x + c,
                to_y,
                bj_bitmap_pixel_value(p_dest, red, green, blue)
            );
        }
    }
    return 1;
}

int bj_bitmap_clipped_blit_stretched(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    const bj_rect* p_dest_rect
) {
    // This is the "stupid blit". Just to test clipping works
    bj_check_or_0(p_src);
    bj_check_or_0(p_dest);
    bj_check_or_0(p_src_rect);
    bj_check_or_0(p_dest_rect);
    bj_check_or_0(p_src_rect->w > 0 && p_src_rect->h > 0);
    bj_check_or_0(p_dest_rect->w > 0 && p_dest_rect->h > 0);

    const double scale_x = (double)p_src_rect->w / p_dest_rect->w;
    const double scale_y = (double)p_src_rect->h / p_dest_rect->h;

    uint8_t red = 0, green = 0, blue = 0;

    for (int dy = 0; dy < p_dest_rect->h; ++dy) {
        int sy = (int)(p_src_rect->y + dy * scale_y);
        if (sy < 0 || sy >= (int)p_src->height) continue;

        for (int dx = 0; dx < p_dest_rect->w; ++dx) {
            int sx = (int)(p_src_rect->x + dx * scale_x);
            if (sx < 0 || sx >= (int)p_src->width) continue;

            bj_bitmap_rgb(p_src, sx, sy, &red, &green, &blue);
            bj_bitmap_put_pixel(
                p_dest,
                p_dest_rect->x + dx,
                p_dest_rect->y + dy,
                bj_bitmap_pixel_value(p_dest, red, green, blue)
            );
        }
    }

    return 1;
}

bj_bool bj_bitmap_blit(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    const bj_rect* p_dest_rect
) {

    
    bj_check_or_0(p_src);
    bj_check_or_0(p_dest);

    bj_rect src_rect = {.x = 0, .y = 0, .w = p_src->width, .h = p_src->height};
    bj_rect dest_rect = { 0 };

    if (p_dest_rect) {
        dest_rect.x = p_dest_rect->x;
        dest_rect.y = p_dest_rect->y;
    }

    // Clip source area
    if (p_src_rect) {
        bj_rect intersection = { 0 };
        if (bj_rect_intersect(p_src_rect, &src_rect, &intersection) == 0) {
            return 0;
        }
        dest_rect.x += intersection.x - p_src_rect->x;
        dest_rect.y += intersection.y - p_src_rect->y;
        src_rect = intersection;
    }

    dest_rect.w = src_rect.w;
    dest_rect.h = src_rect.h;

    bj_rect intersection = { 0 };
    if (bj_rect_intersect(&dest_rect, &(bj_rect){.x = 0, .y = 0, .w = p_dest->width, .h = p_dest->height}, & intersection) == 0) {
        return 0;
    }

    // Adjust src rect
    src_rect.x += intersection.x - dest_rect.x;
    src_rect.y += intersection.y - dest_rect.y;
    src_rect.w = intersection.w;
    src_rect.h = intersection.h;
    dest_rect = intersection;

    if (dest_rect.w <= 0 || dest_rect.h <= 0) {
        return 0;
    }

    return bj_bitmap_clipped_blit(p_src, &src_rect, p_dest, &dest_rect);

}

bj_bool bj_bitmap_blit_stretched(
    const bj_bitmap* p_src,
    const bj_rect* p_src_rect,
    bj_bitmap* p_dest,
    const bj_rect* p_dest_rect
) {
    if (!p_src || !p_dest) return BJ_FALSE;

    int src_w = p_src_rect ? p_src_rect->w : p_src->width;
    int src_h = p_src_rect ? p_src_rect->h : p_src->height;
    int dest_w = p_dest_rect ? p_dest_rect->w : p_dest->width;
    int dest_h = p_dest_rect ? p_dest_rect->h : p_dest->height;

    if (src_w <= 0 || src_h <= 0 || dest_w <= 0 || dest_h <= 0)
        return BJ_FALSE;

    if (dest_w == src_w && dest_h == src_h) {
        return bj_bitmap_blit(p_src, p_src_rect, p_dest, p_dest_rect);
    }

    const double scale_w = (double)dest_w / src_w;
    const double scale_h = (double)dest_h / src_h;

    double src_x0 = p_src_rect ? p_src_rect->x : 0.0;
    double src_y0 = p_src_rect ? p_src_rect->y : 0.0;
    double src_x1 = src_x0 + src_w;
    double src_y1 = src_y0 + src_h;

    double dst_x0 = p_dest_rect ? p_dest_rect->x : 0.0;
    double dst_y0 = p_dest_rect ? p_dest_rect->y : 0.0;
    double dst_x1 = dst_x0 + dest_w;
    double dst_y1 = dst_y0 + dest_h;

    if (src_x0 < 0) {
        dst_x0 -= src_x0 * scale_w;
        src_x0 = 0;
    }
    if (src_y0 < 0) {
        dst_y0 -= src_y0 * scale_h;
        src_y0 = 0;
    }
    if (src_x1 > p_src->width) {
        dst_x1 -= (src_x1 - p_src->width) * scale_w;
        src_x1 = p_src->width;
    }
    if (src_y1 > p_src->height) {
        dst_y1 -= (src_y1 - p_src->height) * scale_h;
        src_y1 = p_src->height;
    }

    const bj_rect clip_rect = { 0, 0, p_dest->width, p_dest->height };

    double adj;

    adj = dst_x0 - clip_rect.x;
    if (adj < 0) {
        src_x0 -= adj / scale_w;
        dst_x0 = clip_rect.x;
    }

    adj = dst_y0 - clip_rect.y;
    if (adj < 0) {
        src_y0 -= adj / scale_h;
        dst_y0 = clip_rect.y;
    }

    adj = dst_x1 - (clip_rect.x + clip_rect.w);
    if (adj > 0) {
        src_x1 -= adj / scale_w;
        dst_x1 = clip_rect.x + clip_rect.w;
    }

    adj = dst_y1 - (clip_rect.y + clip_rect.h);
    if (adj > 0) {
        src_y1 -= adj / scale_h;
        dst_y1 = clip_rect.y + clip_rect.h;
    }

    bj_rect clipped_src = {
        .x = (int)bj_round(src_x0),
        .y = (int)bj_round(src_y0),
        .w = (int)bj_round(src_x1 - src_x0),
        .h = (int)bj_round(src_y1 - src_y0),
    };

    bj_rect clipped_dest = {
        .x = (int)bj_round(dst_x0),
        .y = (int)bj_round(dst_y0),
        .w = (int)bj_round(dst_x1 - dst_x0),
        .h = (int)bj_round(dst_y1 - dst_y0),
    };

    if (clipped_src.w <= 0 || clipped_src.h <= 0 ||
        clipped_dest.w <= 0 || clipped_dest.h <= 0) {
        return BJ_FALSE;
    }

    bj_rect src_bounds = { 0, 0, p_src->width, p_src->height };
    if (bj_rect_intersect(&src_bounds, &clipped_src, &clipped_src) == 0) {
        return BJ_FALSE;
    }

    if (bj_rect_intersect(&clip_rect, &clipped_dest, &clipped_dest) == 0) {
        return BJ_FALSE;
    }

    return bj_bitmap_clipped_blit_stretched(p_src, &clipped_src, p_dest, &clipped_dest);
}
