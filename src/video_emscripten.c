#include "config.h"

#if BJ_HAS_FEATURE(EMSCRIPTEN)

#include <banjo/event.h>
#include <banjo/video.h>

#include "check.h"
#include "window_t.h"

#include <emscripten/html5.h>

#define BJ_EM_CANVAS_SELECTOR "#canvas"

struct {
    bj_bool window_exist;
} emscripten;

typedef struct {
    struct bj_window_t common;
    const char* selector;
    int         width;
    int         height;
} emscripten_window;

bool em_mouse_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    const int x = mouseEvent->targetX;
    const int y = mouseEvent->targetY;

    switch (eventType) {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
        case EMSCRIPTEN_EVENT_MOUSEUP: {
            const int action = (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) ? BJ_PRESS : BJ_RELEASE;
            // +1 because EMSCRITEN buttons are the same as ours -1
            bj_push_button_event(userData, mouseEvent->button + 1, action, x, y);
            break;
        }
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            bj_push_cursor_event(userData, x, y);
            break;
        case EMSCRIPTEN_EVENT_MOUSEENTER:
        case EMSCRIPTEN_EVENT_MOUSELEAVE: {
            bj_push_enter_event(userData, eventType == EMSCRIPTEN_EVENT_MOUSEENTER, x, y);
            break;
        }
        case EMSCRIPTEN_EVENT_CLICK:
        case EMSCRIPTEN_EVENT_DBLCLICK:
        default:
            break;
    }

    return 0;
}

static bj_window* emscripten_window_new(
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

    if(emscripten.window_exist == BJ_TRUE) {
        return 0;
    }

    emscripten_window window = { 
        .common = {
            .flags = flags,
        },
        .selector = BJ_EM_CANVAS_SELECTOR,
        .width    = width,
        .height   = height,
    };

    EMSCRIPTEN_RESULT res = emscripten_set_canvas_element_size(window.selector, width, height);
    if(res != EMSCRIPTEN_RESULT_SUCCESS) {
        return 0;
    }

    emscripten_window* p_window = bj_malloc(sizeof(emscripten_window));

    emscripten_set_mousedown_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mouseup_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mousemove_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mouseenter_callback(window.selector, p_window, 0, em_mouse_callback);
    emscripten_set_mouseleave_callback(window.selector, p_window, 0, em_mouse_callback);

    bj_memcpy(p_window, &window, sizeof(emscripten_window));
    return (bj_window*)p_window;
}

static void emscripten_window_del(
    bj_video_layer* p_layer,
    bj_window* p_abstract_window
) {
    (void)p_layer;
    emscripten_window* p_window = (emscripten_window*)p_abstract_window;
    emscripten_set_canvas_element_size(p_window->selector, 0, 0);
    emscripten.window_exist = BJ_FALSE;

    bj_free(p_window);
}

static void emscripten_end_layer(
    bj_video_layer* p_layer,
    bj_error** p_error
) {
    (void)p_error;
    bj_free(p_layer);
}

static void emscripten_window_poll(
    bj_video_layer* p_layer
) {
    (void)p_layer;
}

static int emscripten_get_window_size(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    int* width,
    int* height
) {
    (void)p_layer;
    bj_check_or_0(p_abstract_window);
    bj_check_or_0(width || height);
    const emscripten_window* p_window = (const emscripten_window*)p_abstract_window;
    if(width) {
        *width = p_window->width;
    }
    if(height) {
        *height = p_window->height;
    }
    return 1;
}

static bj_bitmap* emscripten_create_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window* p_abstract_window,
    bj_error** p_error
) {
    (void)p_layer;
    (void)p_error;
    emscripten_window* p_window = (emscripten_window*)p_abstract_window;
    return bj_bitmap_new(
        p_window->width, p_window->height,
        BJ_PIXEL_MODE_INDEXED_1, 0
    );
}

static void emscripten_flush_window_framebuffer(
    bj_video_layer* p_layer,
    const bj_window*   p_abstract_window
) {
    (void)p_layer;
    (void)p_abstract_window;
}

static bj_video_layer* emscripten_init_layer(
    bj_error** p_error
) {
    (void)p_error;

    bj_video_layer* p_layer = bj_malloc(sizeof(bj_video_layer));
    p_layer->end                       = emscripten_end_layer;
    p_layer->create_window             = emscripten_window_new;
    p_layer->delete_window             = emscripten_window_del;
    p_layer->poll_events               = emscripten_window_poll;
    p_layer->get_window_size           = emscripten_get_window_size;
    p_layer->create_window_framebuffer = emscripten_create_window_framebuffer;
    p_layer->flush_window_framebuffer  = emscripten_flush_window_framebuffer;
    p_layer->data = 0;
    return p_layer;
}

bj_video_layer_create_info emscripten_layer_info = {
    .name   = "emscripten",
    .create = emscripten_init_layer,
};

#endif
