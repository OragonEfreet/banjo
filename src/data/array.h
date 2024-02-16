#pragma once

#include <banjo/array.h>

typedef struct BjArray_T {
    const BjAllocationCallbacks* pAllocator;
    usize                        value_size;
    usize                        capacity;
    usize                        count;
    void*                        pData;
} BjArray_T;

void bjInitArray(const BjArrayInfo*, const BjAllocationCallbacks*, BjArray);
void bjResetArray(BjArray);

