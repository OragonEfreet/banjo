#pragma once

#include <banjo/app.h>
#include <banjo/window.h>

struct BjWindow_T {
    void*         handle;
    BjApplication application;
};

void bj_ProcessEvents();
BjResult bj_RetainWindowComponent(BjApplication application);
BjResult bj_ReleaseWindowComponent(BjApplication application);

