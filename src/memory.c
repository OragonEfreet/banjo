#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/error.h>

#include "check.h"

#include <stdlib.h> // malloc, realloc, ...
#include <string.h> // memset, memcpy, memmove

static void* fallback_malloc(void* user_data, size_t size) {
    (void)user_data;

    return malloc(size);
}

static void* fallback_realloc(void* user_data, void* original, size_t size) {
    (void)user_data;

    return realloc(original, size);
}

static void fallback_free(void* user_data, void* ptr) {
    (void)user_data;

    free(ptr);
}

static bj_memory_callbacks s_default = {
    .fn_allocation   = fallback_malloc,
    .fn_reallocation = fallback_realloc,
    .fn_free         = fallback_free,
};

void* bj_malloc(size_t size) {
    return s_default.fn_allocation(s_default.p_user_data, size);
}

void* bj_calloc(size_t size) {
    void* ptr = bj_malloc(size);
    bj_memzero(ptr, size);
    return ptr;
}

void* bj_realloc(void* p_memory, size_t size) {
    return s_default.fn_reallocation(s_default.p_user_data, p_memory, size);
}

void bj_free(void* p_memory) {
    s_default.fn_free(s_default.p_user_data, p_memory);
}


void bj_memory_set_defaults(
    const bj_memory_callbacks* p_allocator
) {
    if(p_allocator == 0) {
        s_default = (bj_memory_callbacks) {
            .fn_allocation   = fallback_malloc,
            .fn_reallocation = fallback_realloc,
            .fn_free         = fallback_free,
        };
    } else {
        bj_check(p_allocator != 0);
        s_default = (bj_memory_callbacks) {
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
    size_t       mem_size
) {
    return memcpy(p_dest, p_src, mem_size);
}

void* bj_memmove(
    void*       p_dest,
    const void* p_src,
    size_t       mem_size
) {
    return memmove(p_dest, p_src, mem_size);
}

int bj_memcmp(
    const void* p_block_a,
    const void* p_block_b,
    size_t size
) {
    return memcmp(p_block_a, p_block_b, size);
}

void bj_memset(
    void* p_dest,
    uint8_t    value,
    size_t num
) {
    memset(p_dest, value, num);
}

void bj_memzero(
    void*  p_dest,
    size_t mem_size
) {
    bj_memset(p_dest, 0, mem_size);
}
