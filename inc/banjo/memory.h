#pragma once

#include <banjo/core.h>

typedef void* (BJAPI_PTR *PFN_bjAllocationFunction)(
    void* pUserData,
    usize size
);

typedef void* (BJAPI_PTR *PFN_bjReallocationFunction)(
    void* pUserData,
    void* pOriginal,
    usize size
);

typedef void (BJAPI_PTR *PFN_bjFreeFunction)(
    void* pUserData,
    void* pMemory
);

typedef struct {
    void*                      pUserData;
    PFN_bjAllocationFunction   pfnAllocation;
    PFN_bjReallocationFunction pfnReallocation;
    PFN_bjFreeFunction         pfnFree;
} BjAllocationCallbacks;

BJAPI_ATTR void* BJAPI_CALL bjAllocate(
    usize                        size,
    const BjAllocationCallbacks* pAllocator
);

BJAPI_ATTR void* BJAPI_CALL bjReallocate(
    void*                        pMemory,
    usize                        size,
    const BjAllocationCallbacks* pAllocator
);

BJAPI_ATTR void BJAPI_CALL bjFree(
    void*                         pMemory,
    const BjAllocationCallbacks*  pAllocator
);


