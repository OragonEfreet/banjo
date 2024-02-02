#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjWorld);

typedef struct {
    BjAllocationCallbacks*   pAllocator;
} BjWorldCreateInfo;

BANJO_EXPORT BjResult bjCreateWorld(
    const BjWorldCreateInfo* pCreateInfo,
    BjWorld*                 pInstance
);

BANJO_EXPORT void bjDestroyWorld(
    BjWorld world
);





