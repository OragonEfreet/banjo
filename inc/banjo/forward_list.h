#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjForwardList);

typedef struct BjForwardListInfo {
    usize                  value_size;
    bool                   weak_owning;
} BjForwardListInfo;

BANJO_EXPORT BjForwardList bjCreateForwardList(
    const BjForwardListInfo*     pInfo,
    const BjAllocationCallbacks* pAllocator
);

BANJO_EXPORT void bjDestroyForwardList(
    BjForwardList list
);

BANJO_EXPORT void bjClearForwardList(
    BjForwardList list
);

BANJO_EXPORT usize bjForwardListCount(
    BjForwardList list
);

BANJO_EXPORT void bjForwardListInsert(
    BjForwardList list,
    usize index,
    void* pData
);

BANJO_EXPORT void bjForwardListPrepend(
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




