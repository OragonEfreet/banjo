#pragma once

#include <banjo/memory.h>
#include <banjo/log.h>

#include <stdlib.h>
#include <string.h>

typedef struct  {
    usize actual_max_allocated;
    usize actual_current_allocated;
    usize application_max_allocated;
    usize application_current_allocated;
    u16   n_allocations;
    u16   n_reallocations;
    u16   n_free;
} allocation_data;

#define CHECK_CLEAN_ALLOC(ALLOC) \
    CHECK_EQ(ALLOC.application_current_allocated, 0); \
    CHECK_EQ(ALLOC.actual_current_allocated, 0);

#define REQUIRE_CLEAN_ALLOC(ALLOC) \
    REQUIRE_EQ(ALLOC.application_current_allocated, 0); \
    REQUIRE_EQ(ALLOC.actual_current_allocated, 0);

void logAllocations(allocation_data* data) {
    bjLog(TRACE, "Memory:\n\tApplication allocated:\t %ld (max: %ld)\n\tTotal allocated:\t %ld (max %ld)\n\t%d allocs, %d reallocs, %d frees",
        data->application_current_allocated, data->application_max_allocated,
        data->actual_current_allocated, data->actual_max_allocated,
        data->n_allocations, data->n_reallocations, data->n_free
    );
}

typedef struct {
    usize appsize;
    void* ptr;
} block_allocation;

static usize block_allocation_size = sizeof(block_allocation);

static void* mock_malloc(void* pUserData, usize appsize) {
    if (pUserData == 0) {
        return malloc(appsize);
    }

    usize memsize = appsize + block_allocation_size;

    block_allocation* meta = malloc(memsize);
    memset(meta, 0, memsize);
    meta->appsize = appsize;
    meta->ptr = meta + 1;

    allocation_data* pData = (allocation_data*)pUserData;
    if((pData->actual_current_allocated += memsize) > pData->actual_max_allocated) {
        pData->actual_max_allocated = pData->actual_current_allocated;
    }
    if((pData->application_current_allocated += appsize) > pData->application_max_allocated) {
        pData->application_max_allocated = pData->application_current_allocated;
    }
    pData->n_allocations += 1;

    return meta->ptr;
}

void mock_free(void* pUserData, void* pAppPtr) {
    if(pAppPtr == 0) {
        return;
    }

    if(pUserData == 0) {
        free(pAppPtr);
    } else {

        block_allocation* meta = pAppPtr;
        meta -= 1;

        usize appsize = meta->appsize;
        usize memsize = appsize + block_allocation_size;

        memset(meta, 0, memsize);
        free(meta);

        allocation_data* pData = (allocation_data*)pUserData;
        pData->actual_current_allocated -= memsize;
        pData->application_current_allocated -= appsize;
        pData->n_free += 1;
    }
}

static void* mock_realloc(void* pUserData, void* pAppPtr, usize appsize) {
    if(pUserData == 0) {
        return realloc(pAppPtr, appsize);
    }

    void* res = mock_malloc(pUserData, appsize);
    memcpy(res, pAppPtr, appsize);
    mock_free(pUserData, pAppPtr);

    allocation_data* pData = (allocation_data*)pUserData;
    pData->n_free -= 1;
    pData->n_allocations -= 1;
    pData->n_reallocations += 1;

    return res;
}

BjAllocationCallbacks mock_allocators(allocation_data* pData) {
    return (BjAllocationCallbacks) {
        .pfnAllocation   = mock_malloc,
        .pfnReallocation = mock_realloc,
        .pfnFree         = mock_free,
        .pUserData       = pData,
    };
}

