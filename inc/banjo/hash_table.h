#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

typedef struct BjHashTable_T {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
} BjHashTable_T;

BJ_DEFINE_HANDLE(BjHashTable);

typedef struct BjHashTableCreateInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
} BjHashTableCreateInfo;

BANJO_EXPORT BjResult bjCreateHashTable(
    const BjHashTableCreateInfo* pCreateInfo,
    BjHashTable*                 pInstance
);

BANJO_EXPORT BjResult bjInitHashTable(
    const BjHashTableCreateInfo* pCreateInfo,
    BjHashTable                  pInstance
);

BANJO_EXPORT BjResult bjResetHashTable(
    BjHashTable table
);

BANJO_EXPORT BjResult bjDestroyHashTable(
    BjHashTable table
);

BANJO_EXPORT BjResult bjClearHashTable(
    BjHashTable table
);


