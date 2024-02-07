#pragma once

#include <banjo/app.h>
#include <banjo/core.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjWindow);

typedef struct BjWindowCreateInfo {
    BjApplication application;
    usize         width;
    usize         height;
    const char*   title;
} BjWindowCreateInfo;

BANJO_EXPORT BjResult bjCreateWindow(
    const BjWindowCreateInfo* pCreateInfo,
    BjWindow*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyWindow(
    BjWindow window
);

BANJO_EXPORT bool bjWindowShouldClose(
    BjWindow window
);


    

