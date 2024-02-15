#pragma once

#include <banjo/array.h>

typedef struct BjArray_T {
    BjAllocationCallbacks* pAllocator;
    usize                  value_size;
    usize                  capacity;
    usize                  count;
    void*                  pData;
} BjArray_T;

BjResult bjInitArray( const BjArrayInfo*, BjArray);
BjResult bjResetArray(BjArray);

