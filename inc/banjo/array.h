#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjArray);

typedef struct BjArrayInfo {
    usize                  value_size;
    usize                  count;
    usize                  capacity;
} BjArrayInfo;

BANJO_EXPORT BjArray bjCreateArray(
    const BjArrayInfo*           pInfo,
    const BjAllocationCallbacks* pAllocator
);

BANJO_EXPORT void bjDestroyArray(
    BjArray array
);

BANJO_EXPORT void bjReserveArray(
    BjArray pArray,
    usize   capacity
);





