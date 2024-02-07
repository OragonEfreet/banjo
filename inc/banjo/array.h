#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

typedef struct BjArray_T {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
    usize                  capacity;
    usize                  count;
    void*                  pData;
} BjArray_T;

BJ_DEFINE_HANDLE(BjArray);

typedef struct BjArrayCreateInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
    usize                  capacity;
} BjArrayCreateInfo;

BANJO_EXPORT BjResult bjCreateArray(
    const BjArrayCreateInfo* pCreateInfo,
    BjArray*                 pInstance
);

BANJO_EXPORT BjResult bjInitArray(
    const BjArrayCreateInfo* pCreateInfo,
    BjArray                  pInstance
);

BANJO_EXPORT BjResult bjResetArray(
    BjArray array
);

BANJO_EXPORT BjResult bjDestroyArray(
    BjArray array
);

BANJO_EXPORT BjResult bjReserveArray(
    BjArray pArray,
    usize   capacity
);





