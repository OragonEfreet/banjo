#pragma once

#include <banjo/app.h>
#include <banjo/core.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjWindow);

typedef struct {
    BjApplication application;
    usize         width;
    usize         height;
    const char*   title;
} BjWindowCreateInfo;

BJAPI_ATTR BjResult BJAPI_CALL bjCreateWindow(
    const BjWindowCreateInfo* pCreateInfo,
    BjWindow*                 pInstance
);

BJAPI_ATTR void BJAPI_CALL bjDestroyWindow(
    BjWindow window
);

BJAPI_ATTR bool BJAPI_CALL bjWindowShouldClose(
    BjWindow window
);


    

