#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjArray);

typedef struct BjArrayInfo {
    usize value_size;
    usize count;
    usize capacity;
} BjArrayInfo;

BANJO_EXPORT BjArray bj_array_create(
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_array_destroy(
    BjArray array
);

BANJO_EXPORT void bj_array_reserve(
    BjArray array,
    usize   capacity
);





