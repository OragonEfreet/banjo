#pragma once

#include <banjo/array.h>

typedef struct BjArray_T {
    const BjAllocationCallbacks* p_allocator;
    usize                        value_size;
    usize                        capacity;
    usize                        count;
    void*                        p_data;
} BjArray_T;

void bj_array_init(const BjArrayInfo*, const BjAllocationCallbacks*, BjArray);
void bj_array_reset(BjArray);

