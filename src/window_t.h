#pragma once

#include <banjo/event.h>
#include <banjo/window.h>

struct bj_window_t {
    char                     keystates[0xFF];
    uint8_t                  flags;

    bj_bitmap*               p_framebuffer;
};




