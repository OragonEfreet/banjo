#pragma once

#include <banjo/app.h>
#include <banjo/window.h>

typedef struct BjApplication_T {
    char*                  name;
    BjAllocationCallbacks* p_allocator;
    bool                   shouldClose;
    usize                  windowCount;
    BjWindow*              p_windows;
} BjApplication_T;

void bj_window_link(
    BjApplication                application,
    BjWindow                     window
);

void bj_window_unlink(
    BjApplication                application,
    BjWindow                     window
);

