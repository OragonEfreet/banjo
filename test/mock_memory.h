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
} sAllocationData;

#define CHECK_CLEAN_ALLOC(ALLOC) \
    CHECK_EQ(ALLOC.application_current_allocated, 0); \
    CHECK_EQ(ALLOC.actual_current_allocated, 0);

#define REQUIRE_CLEAN_ALLOC(ALLOC) \
    REQUIRE_EQ(ALLOC.application_current_allocated, 0); \
    REQUIRE_EQ(ALLOC.actual_current_allocated, 0);

void logAllocations(sAllocationData* data) {
    bj_log(TRACE, "Memory:\n\tApplication allocated:\t %ld (max: %ld)\n\tTotal allocated:\t %ld (max %ld)\n\t%d allocs, %d reallocs, %d frees",
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

static void* mock_malloc(void* p_user_data, usize appsize) {
    if (p_user_data == 0) {
        return malloc(appsize);
    }

    usize memsize = appsize + block_allocation_size;

    block_allocation* meta = malloc(memsize);
    memset(meta, 0, memsize);
    meta->appsize = appsize;
    meta->ptr = meta + 1;

    sAllocationData* pData = (sAllocationData*)p_user_data;
    if((pData->actual_current_allocated += memsize) > pData->actual_max_allocated) {
        pData->actual_max_allocated = pData->actual_current_allocated;
    }
    if((pData->application_current_allocated += appsize) > pData->application_max_allocated) {
        pData->application_max_allocated = pData->application_current_allocated;
    }
    pData->n_allocations += 1;

    /* bjLog(TRACE, "Allocated %ld bytes @ %p", meta->appsize, meta->ptr); */ 

    return meta->ptr;
}

void mock_free(void* p_user_data, void* pAppPtr) {
    if(pAppPtr == 0) {
        return;
    }

    if(p_user_data == 0) {
        free(pAppPtr);
    } else {

        block_allocation* meta = pAppPtr;
        meta -= 1;

        usize appsize = meta->appsize;
        usize memsize = appsize + block_allocation_size;

        memset(meta, 0, memsize);
        free(meta);

        sAllocationData* pData = (sAllocationData*)p_user_data;
        pData->actual_current_allocated -= memsize;
        pData->application_current_allocated -= appsize;
        pData->n_free += 1;

        /* bjLog(TRACE, "Freed %ld bytes @ %p", appsize, pAppPtr); */ 
    }
}

static void* mock_realloc(void* p_user_data, void* pAppPtr, usize appsize) {
    if(p_user_data == 0) {
        return realloc(pAppPtr, appsize);
    }

    void* res = mock_malloc(p_user_data, appsize);
    bj_memcpy(res, pAppPtr, appsize);
    mock_free(p_user_data, pAppPtr);

    sAllocationData* pData = (sAllocationData*)p_user_data;
    pData->n_free -= 1;
    pData->n_allocations -= 1;
    pData->n_reallocations += 1;

    return res;
}

BjAllocationCallbacks mock_allocators(sAllocationData* pData) {
    return (BjAllocationCallbacks) {
        .p_user_data     = pData,
        .fn_allocation   = mock_malloc,
        .fn_reallocation = mock_realloc,
        .fn_free         = mock_free,
    };
}

