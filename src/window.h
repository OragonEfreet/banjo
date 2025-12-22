#pragma once

#include <banjo/event.h>
#include <banjo/window.h>

struct bj_window {
    char                     keystates[0xFF];
    uint8_t                  flags;

    struct bj_bitmap*               framebuffer;
};




