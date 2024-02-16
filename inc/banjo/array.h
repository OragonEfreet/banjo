#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjArray);

typedef struct BjArrayInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  value_size;
    usize                  count;
    usize                  capacity;
} BjArrayInfo;

BANJO_EXPORT BjArray bjCreateArray(
    const BjArrayInfo* pInfo
);

BANJO_EXPORT void bjDestroyArray(
    BjArray array
);

BANJO_EXPORT void bjReserveArray(
    BjArray pArray,
    usize   capacity
);





