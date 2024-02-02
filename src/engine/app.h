#pragma once

#include <banjo/app.h>
#include <banjo/window.h>

struct BjApplication_T {
    char*                  name;
    BjAllocationCallbacks* pAllocator;
    bool                   shouldClose;
    usize                  windowCount;
    BjWindow*              pWindows;
};

void bj_LinkWindow(
    BjApplication                application,
    BjWindow                     window
);

void bj_UnlinkWindow(
    BjApplication                application,
    BjWindow                     window
);

