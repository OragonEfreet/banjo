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

BANJO_EXPORT BjResult bjCreateArray(
    const BjArrayInfo* pInfo,
    BjArray*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyArray(
    BjArray array
);

BANJO_EXPORT BjResult bjReserveArray(
    BjArray pArray,
    usize   capacity
);





