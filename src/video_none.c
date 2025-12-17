
#include "check.h"
#include "video_layer.h"
#include "window.h"

struct novideo_window {
    struct bj_window common;
    int        width;
    int        height;
};

static struct bj_window* novideo_window_new(
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
) {
    (void)p_title;
    (void)x;
    (void)y;

    flags |= BJ_WINDOW_FLAG_CLOSE;

    struct novideo_window window = { 
        .common = {
            .flags = flags,
        },
        .width = width,
        .height = height,
    };

    struct novideo_window* p_window = bj_malloc(sizeof(struct novideo_window));
    bj_memcpy(p_window, &window, sizeof(struct novideo_window));
    return (struct bj_window*)p_window;
}

static void novideo_window_del(
    struct bj_window* p_abstract_window
) {
    struct novideo_window* p_window = (struct novideo_window*)p_abstract_window;
    bj_free(p_window);
}

static void novideo_window_poll(
    void
) {
    // EMPTY
}

static int novideo_get_window_size(
    const struct bj_window* p_abstract_window,
    int* width,
    int* height
) {
    bj_check_or_0(p_abstract_window);
    bj_check_or_0(width || height);
    const struct novideo_window* p_window = (const struct novideo_window*)p_abstract_window;
    if(width) {
        *width = p_window->width;
    }
    if(height) {
        *height = p_window->height;
    }
    return 1;
}

static void novideo_dispose_layer(
    struct bj_error**      p_error
) {
    (void)p_error;
}


static bj_bool novideo_create_layer(
    struct bj_video_layer* layer,
    struct bj_error**      error
) {
    (void)error;

    layer->end                       = novideo_dispose_layer;
    layer->create_window             = novideo_window_new;
    layer->delete_window             = novideo_window_del;
    layer->poll_events               = novideo_window_poll;
    layer->get_window_size           = novideo_get_window_size;

    return BJ_TRUE;
}

struct bj_video_layer_create_info novideo_video_layer_info = {
    .name = "novideo",
    .create = novideo_create_layer,
};

