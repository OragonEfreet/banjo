#pragma once

#include <banjo/app.h>
#include <banjo/window.h>

typedef struct BjWindow_T {
    void*         handle;
    BjApplication application;
} BjWindow_T;

void bj_ProcessEvents();
BjResult bj_RetainWindowComponent(BjApplication application);
BjResult bj_ReleaseWindowComponent(BjApplication application);

