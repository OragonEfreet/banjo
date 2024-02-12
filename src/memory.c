#include <banjo/log.h>
#include <banjo/memory.h>
#include <errors.h>

#include <stdlib.h>

static inline void* fallback_malloc(void* user_data, usize size) {
    return malloc(size);
}

static inline void* fallback_realloc(void* user_data, void* original, usize size) {
    return realloc(original, size);
}

static inline void fallback_free(void* user_data, void* ptr) {
    free(ptr);
}

static BjAllocationCallbacks s_default = {
    .pfnAllocation   = fallback_malloc,
    .pfnReallocation = fallback_realloc,
    .pfnFree         = fallback_free,
};

BANJO_EXPORT void* bjAllocate(
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    const BjAllocationCallbacks* allocator = (pAllocator == 0 ? &s_default : pAllocator);
    return allocator->pfnAllocation(allocator->pUserData, size);
}

BANJO_EXPORT void* bjReallocate(
    void*                        pMemory,
    usize                        size,
    const BjAllocationCallbacks* pAllocator
) {
    const BjAllocationCallbacks* allocator = (pAllocator == 0 ? &s_default : pAllocator);
    return allocator->pfnReallocation(allocator->pUserData, pMemory, size);
}

BANJO_EXPORT void bjFree(
    void*                         pMemory,
    const BjAllocationCallbacks*  pAllocator
) {
    const BjAllocationCallbacks* allocator = (pAllocator == 0 ? &s_default : pAllocator);
    allocator->pfnFree(allocator->pUserData, pMemory);
}


BjResult bjSetDefaultAllocator(
    const BjAllocationCallbacks* pAllocator
) {
    if(pAllocator == 0) {
        s_default = (BjAllocationCallbacks) {
            .pfnAllocation   = fallback_malloc,
            .pfnReallocation = fallback_realloc,
            .pfnFree         = fallback_free,
        };
    } else {
        bjExpectValue(pAllocator->pfnAllocation, BJ_INVALID_PARAMETER);
        bjExpectValue(pAllocator->pfnReallocation, BJ_INVALID_PARAMETER);
        bjExpectValue(pAllocator->pfnFree, BJ_INVALID_PARAMETER);
        s_default = (BjAllocationCallbacks) {
            .pfnAllocation   = pAllocator->pfnAllocation,
            .pfnReallocation = pAllocator->pfnReallocation,
            .pfnFree         = pAllocator->pfnFree,
            .pUserData       = pAllocator->pUserData,
        };
    }

    return BJ_SUCCESS;
}

BANJO_EXPORT BjResult bjUnsetDefaultAllocator(void) {
    return bjSetDefaultAllocator(0);
}
