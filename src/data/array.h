#pragma once

#include <banjo/array.h>

typedef struct BjArray_T {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
    usize                  capacity;
    usize                  count;
    void*                  pData;
} BjArray_T;

BjResult bjInitArray( const BjArrayCreateInfo*, BjArray);
BjResult bjResetArray(BjArray);

