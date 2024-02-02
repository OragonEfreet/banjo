#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjApplication);

typedef struct {
    const char*            name;
    BjAllocationCallbacks* pAllocator;
} BjApplicationCreateInfo;

BANJO_EXPORT BjResult bjCreateApplication(
    const BjApplicationCreateInfo* pCreateInfo,
    BjApplication*                 pApplication
);

BANJO_EXPORT void bjDestroyApplication(
    BjApplication application
);

BANJO_EXPORT bool bjApplicationShouldClose(
    BjApplication application
);

BANJO_EXPORT void bjCloseApplication(
    BjApplication application
);

BANJO_EXPORT void bjTickApplication(
    BjApplication application
);



    

