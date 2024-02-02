#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjWorld);

typedef struct {
    BjAllocationCallbacks*   pAllocator;
} BjWorldCreateInfo;

BJAPI_ATTR BjResult BJAPI_CALL bjCreateWorld(
    const BjWorldCreateInfo* pCreateInfo,
    BjWorld*                 pInstance
);

BJAPI_ATTR void BJAPI_CALL bjDestroyWorld(
    BjWorld world
);





