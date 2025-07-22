#include <banjo/video.h>

#include "check.h"
#include "window_t.h"

extern bj_video_layer* s_video;

bj_window* bj_window_new(
    const char* p_title,
    uint16_t    x,
    uint16_t    y,
    uint16_t    width,
    uint16_t    height,
    uint8_t     window_flags
) {
    return s_video->create_window(s_video, p_title, x, y, width, height, window_flags);
}

void bj_window_del(
    bj_window* p_window
) {
    bj_check(p_window);
    bj_bitmap_del(p_window->p_framebuffer);
    s_video->delete_window(s_video, p_window);
}

bj_bool bj_window_should_close(
    bj_window* p_window
) {
    return bj_window_get_flags(p_window, BJ_WINDOW_FLAG_CLOSE) > 0;
}

void bj_window_set_should_close(
    bj_window* p_window
) {
    bj_check(p_window);
    p_window->flags |= BJ_WINDOW_FLAG_CLOSE;
}

uint8_t bj_window_get_flags(
    bj_window* p_window,
    uint8_t    flags
) {
    bj_check_or_0(p_window);
    return (p_window->flags & flags);
}

bj_bitmap* bj_window_get_framebuffer(
    bj_window* p_window,
    bj_error**       p_error
) {
    bj_check_or_0(p_window);

    if (p_window->p_framebuffer == 0) {
        
        int width = 0;
        int height = 0;

        if (!bj_window_get_size(p_window, &width, &height)) {
            return 0;
        }

        p_window->p_framebuffer = s_video->create_window_framebuffer(s_video, p_window, p_error);
    }

    return p_window->p_framebuffer;
}

void bj_window_update_framebuffer(
    bj_window* p_window
) {
    bj_check(p_window);
    bj_check(p_window->p_framebuffer != 0);
    s_video->flush_window_framebuffer(s_video, p_window);
}

int bj_window_get_size(
    const bj_window* p_window,
    int* width,
    int* height
) {
    bj_check_or_0(p_window);
    return s_video->get_window_size(s_video, p_window, width, height);
}
