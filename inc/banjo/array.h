#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

typedef struct BjArray_T {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
    usize                  capacity;
    usize                  count;
    void*                  data;
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

BANJO_EXPORT BjResult bjDestroyArray(
    BjArray array
);




