#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct BjForwardListEntry {
    void* value;
    struct BjForwardListEntry* pNext;
} BjForwardListEntry;

typedef struct BjForwardList_T {
    BjAllocationCallbacks*     pAllocator;
    usize                      elem_size;
    struct BjForwardListEntry* pFirstEntry;
} BjForwardList_T;

BJ_DEFINE_HANDLE(BjForwardList);

typedef struct BjForwardListCreateInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
} BjForwardListCreateInfo;

BANJO_EXPORT BjResult bjCreateForwardList(
    const BjForwardListCreateInfo* pCreateInfo,
    BjForwardList*                 pInstance
);

BANJO_EXPORT BjResult bjInitForwardList(
    const BjForwardListCreateInfo* pCreateInfo,
    BjForwardList                  pInstance
);

BANJO_EXPORT BjResult bjResetForwardList(
    BjForwardList list
);

BANJO_EXPORT BjResult bjDestroyForwardList(
    BjForwardList list
);





