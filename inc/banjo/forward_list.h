#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjForwardList);

typedef struct BjForwardListCreateInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  elem_size;
    bool                   weak_owning;
} BjForwardListCreateInfo;

BANJO_EXPORT BjResult bjCreateForwardList(
    const BjForwardListCreateInfo* pCreateInfo,
    BjForwardList*                 pInstance
);

BANJO_EXPORT BjResult bjDestroyForwardList(
    BjForwardList list
);

BANJO_EXPORT BjResult bjClearForwardList(
    BjForwardList list
);

BANJO_EXPORT usize bjForwardListCount(
    BjForwardList list
);

BANJO_EXPORT BjResult bjForwardListInsert(
    BjForwardList list,
    usize index,
    void* pData
);

BANJO_EXPORT BjResult bjForwardListPrepend(
    BjForwardList list,
    void* pData
);

BANJO_EXPORT void* bjForwardListValue(
    BjForwardList list,
    usize index
);

BANJO_EXPORT void* bjForwardListHead(
    BjForwardList list
);




