#pragma once

#include <banjo/forward_list.h>

typedef struct BjForwardListEntry {
    void* value;
    struct BjForwardListEntry* pNext;
} BjForwardListEntry;

typedef struct BjForwardList_T {
    BjAllocationCallbacks*     pAllocator;
    usize                      value_size;
    bool                       weak_owning;
    struct BjForwardListEntry* pHead;
} BjForwardList_T;

BjResult bjInitForwardList(const BjForwardListInfo*, BjForwardList);
BjResult bjResetForwardList(BjForwardList);
