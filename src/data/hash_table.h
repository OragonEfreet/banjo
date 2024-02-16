#pragma once

#include <banjo/hash_table.h>
#include <data/array.h>
#include <data/forward_list.h>

typedef struct BjHashTable_T {
    BjAllocationCallbacks* pAllocator;
    BjArray_T              buckets_array;
    bool                   weak_owning;
    usize                  value_size;
    usize                  key_size;
    PFN_bjHashFunction     pfnHash;
} BjHashTable_T;

void bjInitHashTable(const BjHashTableInfo*, BjHashTable);
void bjResetHashTable(BjHashTable);
