#pragma once

#include <banjo/app.h>
#include <banjo/window.h>

typedef struct BjApplication_T {
    char*                  name;
    BjAllocationCallbacks* pAllocator;
    bool                   shouldClose;
    usize                  windowCount;
    BjWindow*              pWindows;
} BjApplication_T;

void bj_LinkWindow(
    BjApplication                application,
    BjWindow                     window
);

void bj_UnlinkWindow(
    BjApplication                application,
    BjWindow                     window
);

