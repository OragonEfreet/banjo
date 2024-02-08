#pragma once

#include <banjo/hash_table.h>
#include <data/array.h>

typedef struct BjHashTable_T {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
    BjArray_T              buckets;
} BjHashTable_T;

BjResult bjInitHashTable(const BjHashTableCreateInfo*, BjHashTable);
BjResult bjResetHashTable(BjHashTable);
