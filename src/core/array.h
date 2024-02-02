#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

typedef struct BjArray {
    BjAllocationCallbacks* pAllocator;
    void*                  data;
    usize                  capacity;
    usize                  count;
} BjArray;

BANJO_EXPORT BjResult bjInitArray(
    BjArray* array
);

BANJO_EXPORT BjResult bjDropArray(
    BjArray* array
);




