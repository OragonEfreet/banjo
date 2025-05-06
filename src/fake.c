#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "check.h"
#include "system_t.h"
#include "window_t.h"

typedef struct {
    struct bj_window_t common;
    int        width;
    int        height;
} fake_window;

static bj_window* fake_window_new(
    bj_system_backend* p_backend,
    const char* p_title,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint8_t  flags
) {
    (void)p_backend;
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
    bj_system_backend* p_backend,
    bj_window* p_abstract_window
) {
    (void)p_backend;
    fake_window* p_window = (fake_window*)p_abstract_window;
    bj_free(p_window);
}

static void fake_dispose_backend(
    bj_system_backend* p_backend,
    bj_error** p_error
) {
    (void)p_error;

    bj_free(p_backend);
}

static void fake_window_poll(
    bj_system_backend* p_backend
) {
    (void)p_backend;
}

static int fake_get_window_size(
    bj_system_backend* p_backend,
    const bj_window* p_abstract_window,
    int* width,
    int* height
) {
    (void)p_backend;
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
    bj_system_backend* p_backend,
    const bj_window* p_abstract_window,
    bj_error** p_error
) {
    (void)p_backend;
    (void)p_error;
    fake_window* p_window = (fake_window*)p_abstract_window;
    return bj_bitmap_new(
        p_window->width, p_window->height,
        BJ_PIXEL_MODE_INDEXED_1, 0
    );
}

static void fake_flush_window_framebuffer(
    bj_system_backend* p_backend,
    const bj_window*   p_abstract_window
) {
    (void)p_backend;
    (void)p_abstract_window;
}

static bj_system_backend* fake_init_backend(
    bj_error** p_error
) {
    (void)p_error;

    bj_system_backend* p_backend = bj_malloc(sizeof(bj_system_backend));
    p_backend->dispose                   = fake_dispose_backend;
    p_backend->create_window             = fake_window_new;
    p_backend->delete_window             = fake_window_del;
    p_backend->poll_events               = fake_window_poll;
    p_backend->get_window_size           = fake_get_window_size;
    p_backend->create_window_framebuffer = fake_create_window_framebuffer;
    p_backend->flush_window_framebuffer  = fake_flush_window_framebuffer;
    return p_backend;
}

bj_system_backend_create_info fake_backend_create_info = {
    .name = "Fake",
    .create = fake_init_backend,
};

