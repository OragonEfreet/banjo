#include <banjo/memory.h>

#include <check.h>

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

static struct bj_memory_callbacks s_default = {
    .fn_allocation   = fallback_malloc,
    .fn_reallocation = fallback_realloc,
    .fn_free         = fallback_free,
};

void* bj_malloc(size_t size) {
    return s_default.fn_allocation(s_default.user_data, size);
}

void* bj_calloc(size_t size) {
    void* ptr = bj_malloc(size);
    if (ptr) {
        bj_memzero(ptr, size);
    }
    return ptr;
}

void* bj_realloc(void* memory, size_t size) {
    return s_default.fn_reallocation(s_default.user_data, memory, size);
}

void bj_free(void* memory) {
    s_default.fn_free(s_default.user_data, memory);
}


void bj_set_memory_defaults(
    const struct bj_memory_callbacks* allocator
) {
    if(allocator == 0) {
        s_default = (struct bj_memory_callbacks) {
            .fn_allocation   = fallback_malloc,
            .fn_reallocation = fallback_realloc,
            .fn_free         = fallback_free,
        };
    } else {
        bj_check(allocator != 0);
        s_default = (struct bj_memory_callbacks) {
            .fn_allocation   = allocator->fn_allocation,
            .fn_reallocation = allocator->fn_reallocation,
            .fn_free         = allocator->fn_free,
            .user_data       = allocator->user_data,
        };
    }
}

void bj_unset_memory_defaults(void) {
    bj_set_memory_defaults(0);
}

void* bj_memcpy(
    void*       dest,
    const void* src,
    size_t       mem_size
) {
    return memcpy(dest, src, mem_size);
}

void* bj_memmove(
    void*       dest,
    const void* src,
    size_t       mem_size
) {
    return memmove(dest, src, mem_size);
}

int bj_memcmp(
    const void* block_a,
    const void* block_b,
    size_t size
) {
    return memcmp(block_a, block_b, size);
}

void bj_memset(
    void* dest,
    uint8_t    value,
    size_t num
) {
    memset(dest, value, num);
}

void bj_memzero(
    void*  dest,
    size_t mem_size
) {
    bj_memset(dest, 0, mem_size);
}
