#include <banjo/video.h>

#include "check.h"
#include "window_t.h"

typedef struct {
    struct bj_window_t common;
    int        width;
    int        height;
} novideo_window;

static bj_window* novideo_window_new(
    bj_video_layer* p_layer,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
) {
    (void)p_layer;
    (void)p_title;
    (void)x;
    (void)y;

    flags |= BJ_WINDOW_FLAG_CLOSE;

    novideo_window window = { 
        .common = {
            .flags = flags,
        },
        .width = width,
        .height = height,
    };

    novideo_window* p_window = bj_malloc(sizeof(novideo_window));
    bj_memcpy(p_window, &window, sizeof(novideo_window));
    return (bj_window*)p_window;
}

static void novideo_window_del(
    bj_video_layer* p_layer,
    bj_window* p_abstract_window
) {
    (void)p_layer;
    novideo_window* p_window = (novideo_window*)p_abstract_window;
    bj_free(p_window);
}

static void novideo_dispose_layer(
    bj_video_layer* p_layer,
    bj_error** p_error
) {
    (void)p_error;

    bj_free(p_layer);
}

static void novideo_window_poll(
    bj_video_layer* p_layer
) {
    (void)p_layer;
}

static int novideo_get_window_size(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    int* width,
    int* height
) {
    (void)p_layer;
    bj_check_or_0(p_abstract_window);
    bj_check_or_0(width || height);
    const novideo_window* p_window = (const novideo_window*)p_abstract_window;
    if(width) {
        *width = p_window->width;
    }
    if(height) {
        *height = p_window->height;
    }
    return 1;
}

static bj_bitmap* novideo_create_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    bj_error** p_error
) {
    (void)p_layer;
    (void)p_error;
    novideo_window* p_window = (novideo_window*)p_abstract_window;
    return bj_create_bitmap(
        p_window->width, p_window->height,
        BJ_PIXEL_MODE_INDEXED_1, 0
    );
}

static void novideo_flush_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window*   p_abstract_window
) {
    (void)p_layer;
    (void)p_abstract_window;
}

static bj_video_layer* novideo_init_layer(
    bj_error** p_error
) {
    (void)p_error;

    bj_video_layer* p_layer = bj_malloc(sizeof(bj_video_layer));
    p_layer->end                       = novideo_dispose_layer;
    p_layer->create_window             = novideo_window_new;
    p_layer->delete_window             = novideo_window_del;
    p_layer->poll_events               = novideo_window_poll;
    p_layer->get_window_size           = novideo_get_window_size;
    p_layer->create_window_framebuffer = novideo_create_window_framebuffer;
    p_layer->flush_window_framebuffer  = novideo_flush_window_framebuffer;
    p_layer->data = 0;
    return p_layer;
}

bj_video_layer_create_info novideo_video_layer_info = {
    .name = "novideo",
    .create = novideo_init_layer,
};

