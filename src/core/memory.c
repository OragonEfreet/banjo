#include <banjo/memory.h>

#include <stdlib.h>

BANJO_EXPORT void* bjAllocate(
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    if(pAllocator && pAllocator->pfnAllocation) {
        return pAllocator->pfnAllocation(pAllocator->pUserData, size);
    }
    return malloc(size);
}

BANJO_EXPORT void* bjReallocate(
    void*                        pMemory,
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    if(pAllocator && pAllocator->pfnReallocation) {
        return pAllocator->pfnReallocation(pAllocator->pUserData, pMemory, size);
    }
    return realloc(pMemory, size);
}

BANJO_EXPORT void bjFree(
    void*                         pMemory,
    const BjAllocationCallbacks*  pAllocator
) {
    if(pAllocator && pAllocator->pfnFree) {
        pAllocator->pfnFree(pAllocator->pUserData, pMemory);
    }
    free(pMemory);
}

