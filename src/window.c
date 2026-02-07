#include <banjo/event.h>

#include "check.h"
#include "video_layer.h"
#include "window.h"

extern struct bj_video_layer s_video;

struct bj_window* bj_bind_window(
    const char*       title,
    uint16_t          x,
    uint16_t          y,
    uint16_t          width,
    uint16_t          height,
    uint8_t           window_flags,
    struct bj_error** error
) {
    return s_video.create_window(title, x, y, width, height, window_flags, error);
}

void bj_unbind_window(
    struct bj_window* window
) {
    bj_check(window);
    bj_destroy_bitmap(window->framebuffer);
    s_video.delete_window(window);
}

bj_bool bj_should_close_window(
    struct bj_window* window
) {
    return bj_get_window_flags(window, BJ_WINDOW_FLAG_CLOSE) > 0;
}

void bj_set_window_should_close(
    struct bj_window* window
) {
    bj_check(window);
    window->flags |= BJ_WINDOW_FLAG_CLOSE;
}

uint8_t bj_get_window_flags(
    struct bj_window* window,
    uint8_t    flags
) {
    bj_check_or_0(window);
    return (window->flags & flags);
}

int bj_get_window_size(
    const struct bj_window* window,
    int* width,
    int* height
) {
    bj_check_or_0(window);
    return s_video.get_window_size(window, width, height);
}

int bj_get_key(
    const struct bj_window* window,
    int              key
) {
    bj_check_or_return(window, BJ_RELEASE);
    const unsigned uk = (unsigned)key;
    if (uk >= 0xFFu) {
        return BJ_RELEASE;
    }
    return window->keystates[uk];
}
