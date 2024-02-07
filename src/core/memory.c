#include <banjo/log.h>
#include <banjo/memory.h>

#include <stdlib.h>

BANJO_EXPORT void* bjAllocate(
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    void* blocks = 0;
    if(pAllocator && pAllocator->pfnAllocation) {
        blocks = pAllocator->pfnAllocation(pAllocator->pUserData, size);
    }
    blocks = malloc(size);
#ifdef BANJO_PEDANTIC
    bjLog(TRACE, "Allocated %d blocks: %p", size, blocks);
#endif
    return blocks;
}

BANJO_EXPORT void* bjReallocate(
    void*                        pMemory,
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    void* blocks = 0;
    if(pAllocator && pAllocator->pfnReallocation) {
        blocks = pAllocator->pfnReallocation(pAllocator->pUserData, pMemory, size);
    }
    blocks = realloc(pMemory, size);
#ifdef BANJO_PEDANTIC
    bjLog(TRACE, "Reallocated %d blocks from %p: %p", size, pMemory, blocks);
#endif
    return blocks;
}

BANJO_EXPORT void bjFree(
    void*                         pMemory,
    const BjAllocationCallbacks*  pAllocator
) {
#ifdef BANJO_PEDANTIC
    if(pMemory != 0) { bjLog(TRACE, "Freed blocks at %p", pMemory);}
#endif
    if(pAllocator && pAllocator->pfnFree) {
        pAllocator->pfnFree(pAllocator->pUserData, pMemory);
    } else {
        free(pMemory);
    }
}

