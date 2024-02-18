#pragma once

#include <banjo/api.h>

#define bj_new_struct(type, allocator) bj_malloc(sizeof(struct type##_T), allocator)
#define bj_new(type, allocator) bj_malloc(sizeof(type), allocator)

typedef void* (*PFN_bjAllocationFunction)(
    void* p_user_data,
    usize size
);

typedef void* (*PFN_bjReallocationFunction)(
    void* p_user_data,
    void* p_original,
    usize size
);

typedef void (*PFN_bj_freeFunction)(
    void* p_user_data,
    void* p_memory
);

typedef struct BjAllocationCallbacks {
    void*                      p_user_data;
    PFN_bjAllocationFunction   fn_allocation;
    PFN_bjReallocationFunction fn_reallocation;
    PFN_bj_freeFunction         fn_free;
} BjAllocationCallbacks;

BANJO_EXPORT void* bj_malloc(
    usize                        size,
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void* bj_realloc(
    void*                        p_memory,
    usize                        size,
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_free(
    void*                         p_memory,
    const BjAllocationCallbacks*  p_allocator
);

BANJO_EXPORT void bj_memory_set_defaults(
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_memory_unset_defaults(void);

BANJO_EXPORT void bj_memcpy(
    void*       p_dest,
    const void* p_src,
    usize       mem_size
);

BANJO_EXPORT void bj_memset(
    void* p_dest,
    u8    value,
    usize num
);




