#pragma once

#include <banjo/memory.h>

#include <stdlib.h>

static void* mock_malloc(void* data, usize size) {
    return malloc(size);
}

static void* mock_realloc(void* data, void* ptr, usize size) {
    return realloc(ptr, size);
}

void mock_free(void* data, void* ptr) {
    free(ptr);
}

BjAllocationCallbacks mock_allocators() {
    return (BjAllocationCallbacks) {
        .pfnAllocation = mock_malloc,
        .pfnReallocation = mock_realloc,
        .pfnFree = mock_free,
    };
}
