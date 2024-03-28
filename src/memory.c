#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/error.h>

#include <stdlib.h>
#include <string.h>

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
    .fn_allocation   = fallback_malloc,
    .fn_reallocation = fallback_realloc,
    .fn_free         = fallback_free,
};

void* bj_malloc(
    usize                        size,
    const BjAllocationCallbacks* p_allocator
) {
    const BjAllocationCallbacks* allocator = (p_allocator == 0 ? &s_default : p_allocator);
    return allocator->fn_allocation(allocator->p_user_data, size);
}

void* bj_realloc(
    void*                        p_memory,
    usize                        size,
    const BjAllocationCallbacks* p_allocator
) {
    const BjAllocationCallbacks* allocator = (p_allocator == 0 ? &s_default : p_allocator);
    return allocator->fn_reallocation(allocator->p_user_data, p_memory, size);
}

void bj_free(
    void*                         p_memory,
    const BjAllocationCallbacks*  p_allocator
) {
    const BjAllocationCallbacks* allocator = (p_allocator == 0 ? &s_default : p_allocator);
    allocator->fn_free(allocator->p_user_data, p_memory);
}


void bj_memory_set_defaults(
    const BjAllocationCallbacks* p_allocator
) {
    if(p_allocator == 0) {
        s_default = (BjAllocationCallbacks) {
            .fn_allocation   = fallback_malloc,
            .fn_reallocation = fallback_realloc,
            .fn_free         = fallback_free,
        };
    } else {
        bj_assert(p_allocator != 0);
        bj_assert(p_allocator != 0);
        bj_assert(p_allocator != 0);
        s_default = (BjAllocationCallbacks) {
            .fn_allocation   = p_allocator->fn_allocation,
            .fn_reallocation = p_allocator->fn_reallocation,
            .fn_free         = p_allocator->fn_free,
            .p_user_data       = p_allocator->p_user_data,
        };
    }
}

void bj_memory_unset_defaults(void) {
    bj_memory_set_defaults(0);
}

void* bj_memcpy(
    void*       p_dest,
    const void* p_src,
    usize       mem_size
) {
    return memcpy(p_dest, p_src, mem_size);
}

void bj_memset(
    void* p_dest,
    u8    value,
    usize num
) {
    memset(p_dest, value, num);

}
