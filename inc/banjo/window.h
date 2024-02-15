#pragma once

#include <banjo/api.h>
#include <banjo/app.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjWindow);

typedef struct BjWindowInfo {
    BjApplication application;
    usize         width;
    usize         height;
    const char*   title;
} BjWindowInfo;

BANJO_EXPORT BjResult bjCreateWindow(
    const BjWindowInfo* pInfo,
    BjWindow*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyWindow(
    BjWindow window
);

BANJO_EXPORT bool bjWindowShouldClose(
    BjWindow window
);


    

