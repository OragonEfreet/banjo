#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/video.h>

#include "check.h"
#include "window_t.h"

typedef struct {
    struct bj_window_t common;
    int        width;
    int        height;
} fake_window;

static bj_window* fake_window_new(
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

    fake_window window = { 
        .common = {
            .flags = flags,
        },
        .width = width,
        .height = height,
    };

    fake_window* p_window = bj_malloc(sizeof(fake_window));
    bj_memcpy(p_window, &window, sizeof(fake_window));
    return (bj_window*)p_window;
}

static void fake_window_del(
    bj_video_layer* p_layer,
    bj_window* p_abstract_window
) {
    (void)p_layer;
    fake_window* p_window = (fake_window*)p_abstract_window;
    bj_free(p_window);
}

static void fake_dispose_layer(
    bj_video_layer* p_layer,
    bj_error** p_error
) {
    (void)p_error;

    bj_free(p_layer);
}

static void fake_window_poll(
    bj_video_layer* p_layer
) {
    (void)p_layer;
}

static int fake_get_window_size(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    int* width,
    int* height
) {
    (void)p_layer;
    bj_check_or_0(p_abstract_window);
    bj_check_or_0(width || height);
    const fake_window* p_window = (const fake_window*)p_abstract_window;
    if(width) {
        *width = p_window->width;
    }
    if(height) {
        *height = p_window->height;
    }
    return 1;
}

static bj_bitmap* fake_create_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    bj_error** p_error
) {
    (void)p_layer;
    (void)p_error;
    fake_window* p_window = (fake_window*)p_abstract_window;
    return bj_bitmap_new(
        p_window->width, p_window->height,
        BJ_PIXEL_MODE_INDEXED_1, 0
    );
}

static void fake_flush_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window*   p_abstract_window
) {
    (void)p_layer;
    (void)p_abstract_window;
}

static bj_video_layer* fake_init_layer(
    bj_error** p_error
) {
    (void)p_error;

    bj_video_layer* p_layer = bj_malloc(sizeof(bj_video_layer));
    p_layer->dispose                   = fake_dispose_layer;
    p_layer->create_window             = fake_window_new;
    p_layer->delete_window             = fake_window_del;
    p_layer->poll_events               = fake_window_poll;
    p_layer->get_window_size           = fake_get_window_size;
    p_layer->create_window_framebuffer = fake_create_window_framebuffer;
    p_layer->flush_window_framebuffer  = fake_flush_window_framebuffer;
    p_layer->data = 0;
    return p_layer;
}

bj_video_layer_create_info fake_layer_info = {
    .name = "fake",
    .create = fake_init_layer,
};

