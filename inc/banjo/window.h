////////////////////////////////////////////////////////////////////////////////
/// \file
/// Header file for \ref window type.

#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct bj_window_t bj_window;

BANJO_EXPORT bj_window* bj_window_new(
    const char* p_title,
    u16 x,
    u16 y,
    u16 width,
    u16 height
);

BANJO_EXPORT void bj_window_del(
    bj_window* p_window
);


