#pragma once

#include <banjo/core.h>

typedef void* (*PFN_bjAllocationFunction)(
    void* pUserData,
    usize size
);

typedef void* (*PFN_bjReallocationFunction)(
    void* pUserData,
    void* pOriginal,
    usize size
);

typedef void (*PFN_bjFreeFunction)(
    void* pUserData,
    void* pMemory
);

typedef struct BjAllocationCallbacks {
    void*                      pUserData;
    PFN_bjAllocationFunction   pfnAllocation;
    PFN_bjReallocationFunction pfnReallocation;
    PFN_bjFreeFunction         pfnFree;
} BjAllocationCallbacks;

BANJO_EXPORT void* bjAllocate(
    usize                        size,
    const BjAllocationCallbacks* pAllocator
);

BANJO_EXPORT void* bjReallocate(
    void*                        pMemory,
    usize                        size,
    const BjAllocationCallbacks* pAllocator
);

BANJO_EXPORT void bjFree(
    void*                         pMemory,
    const BjAllocationCallbacks*  pAllocator
);


