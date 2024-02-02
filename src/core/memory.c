#include <banjo/memory.h>

#include <stdlib.h>

BJAPI_ATTR void* BJAPI_CALL bjAllocate(
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    if(pAllocator && pAllocator->pfnAllocation) {
        return pAllocator->pfnAllocation(pAllocator->pUserData, size);
    }
    return malloc(size);
}

BJAPI_ATTR void* BJAPI_CALL bjReallocate(
    void*                        pMemory,
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    if(pAllocator && pAllocator->pfnReallocation) {
        return pAllocator->pfnReallocation(pAllocator->pUserData, pMemory, size);
    }
    return realloc(pMemory, size);
}

BJAPI_ATTR void BJAPI_CALL bjFree(
    void*                         pMemory,
    const BjAllocationCallbacks*  pAllocator
) {
    if(pAllocator && pAllocator->pfnFree) {
        pAllocator->pfnFree(pAllocator->pUserData, pMemory);
    }
    free(pMemory);
}

