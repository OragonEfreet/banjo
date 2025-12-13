#include <banjo/log.h>
#include <banjo/renderer.h>

#include "check.h"
#include "renderer_t.h"
#include "video_layer.h"

extern struct bj_video_layer* s_video;

struct bj_renderer* bj_create_renderer(
    enum bj_renderer_type type
) {
    bj_check_or_0(s_video);
    struct bj_renderer* renderer = s_video->create_renderer(s_video, type);
    bj_info("renderer created");
    return renderer;
}

void bj_destroy_renderer(
    struct bj_renderer* renderer
) {
    bj_check(s_video);
    s_video->destroy_renderer(s_video, renderer);
    bj_info("renderer destroyed");
}

void bj_renderer_configure(
    struct bj_renderer* renderer,
    struct bj_window* window
) {
    bj_check(renderer);
    renderer->configure(renderer, window);
}

struct bj_bitmap* bj_get_framebuffer(
    struct bj_renderer* renderer
) {
    bj_check_or_0(renderer);
    return renderer->get_framebuffer ? renderer->get_framebuffer(renderer) : 0;
}

void bj_present(
    struct bj_renderer* renderer,
    struct bj_window*   window
) {
    bj_check(renderer);
    renderer->present(renderer, window);
}

