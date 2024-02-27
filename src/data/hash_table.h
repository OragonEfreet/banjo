#pragma once

#include <banjo/hash_table.h>
#include <data/array.h>
#include <data/list.h>

typedef struct BjHashTable_T {
    const BjAllocationCallbacks* p_allocator;
    BjArray_T                    buckets;
    bool                         weak_owning;
    usize                        value_size;
    usize                        key_size;
    bjHashFunctionPtr            fn_hash;
    usize                        entry_size;
} BjHashTable_T;

void bj_hash_table_init(const BjHashTableInfo*, const BjAllocationCallbacks*, BjHashTable);
void bj_hash_table_reset(BjHashTable);
