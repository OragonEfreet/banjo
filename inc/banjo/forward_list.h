#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjForwardList);

typedef struct BjForwardListInfo {
    BjAllocationCallbacks* pAllocator;
    usize                  value_size;
    bool                   weak_owning;
} BjForwardListInfo;

BANJO_EXPORT BjResult bjCreateForwardList(
    const BjForwardListInfo* pInfo,
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




