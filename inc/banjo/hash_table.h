#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjHashTable);

typedef struct BjHashTableCreateInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
} BjHashTableCreateInfo;

BANJO_EXPORT BjResult bjCreateHashTable(
    const BjHashTableCreateInfo* pCreateInfo,
    BjHashTable*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyHashTable(
    BjHashTable table
);

BANJO_EXPORT BjResult bjClearHashTable(
    BjHashTable table
);



