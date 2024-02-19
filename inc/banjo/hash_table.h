#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjHashTable);

typedef u32 (*PFN_bjHashFunction)(
    const void* p_data, usize size
);

typedef struct BjHashTableInfo {
    usize                  value_size;
    usize                  key_size;
    bool                   weak_owning;
    PFN_bjHashFunction     fn_hash;
} BjHashTableInfo;

BANJO_EXPORT BjHashTable bj_hash_table_create(
    const BjHashTableInfo*       p_info,
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_hash_table_destroy(
    BjHashTable table
);

BANJO_EXPORT void bj_hash_table_clear(
    BjHashTable table
);

BANJO_EXPORT void bj_hash_table_set(
    BjHashTable table,
    void* key,
    void* value
);



