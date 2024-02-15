#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjHashTable);

typedef u32 (*PFN_bjHashFunction)(
    const void* pData, usize size
);

typedef struct BjHashTableInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  value_size;
    usize                  key_size;
    bool                   weak_owning;
    PFN_bjHashFunction     pfnHash;
} BjHashTableInfo;

BANJO_EXPORT BjResult bjCreateHashTable(
    const BjHashTableInfo* pInfo,
    BjHashTable*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyHashTable(
    BjHashTable table
);

BANJO_EXPORT BjResult bjClearHashTable(
    BjHashTable table
);

BANJO_EXPORT BjResult bjHashTableSet(
    BjHashTable table,
    const void* key,
    const void* value
);



