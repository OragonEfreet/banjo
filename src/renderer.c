#include <banjo/log.h>
#include <banjo/renderer.h>

#include "check.h"
#include "renderer.h"
#include "video_layer.h"

extern struct bj_video_layer s_video;

struct bj_renderer* bj_create_renderer(
    enum bj_renderer_type type
) {
    if(type != BJ_RENDERER_TYPE_SOFTWARE) {
        bj_warn("%s %d.%d.%d only supports software rendering.", 
            BJ_NAME, 
            BJ_VERSION_MAJOR_NUMBER,
            BJ_VERSION_MINOR_NUMBER,
            BJ_VERSION_PATCH_NUMBER
        );
        type = BJ_RENDERER_TYPE_SOFTWARE;
    }

    struct bj_renderer* renderer = s_video.create_renderer(type);
    bj_info("renderer created");
    return renderer;
}

void bj_destroy_renderer(
    struct bj_renderer* renderer
) {
    s_video.destroy_renderer(renderer);
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

