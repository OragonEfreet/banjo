#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjApplication);

typedef struct BjApplicationInfo {
    const char*            name;
    BjAllocationCallbacks* pAllocator;
} BjApplicationInfo;

BANJO_EXPORT BjResult bjCreateApplication(
    const BjApplicationInfo* pInfo,
    BjApplication*                 pApplication
);

BANJO_EXPORT BjResult bjDestroyApplication(
    BjApplication application
);

BANJO_EXPORT bool bjApplicationShouldClose(
    BjApplication application
);

BANJO_EXPORT BjResult bjCloseApplication(
    BjApplication application
);

BANJO_EXPORT void bjTickApplication(
    BjApplication application
);



    

