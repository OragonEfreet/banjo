#pragma once

#include <banjo/forward_list.h>

typedef struct BjForwardListEntry {
    void* value;
    struct BjForwardListEntry* pNext;
} BjForwardListEntry;

typedef struct BjForwardList_T {
    BjAllocationCallbacks*     pAllocator;
    usize                      elem_size;
    struct BjForwardListEntry* pFirstEntry;
} BjForwardList_T;

BjResult bjInitForwardList(const BjForwardListCreateInfo*, BjForwardList);
BjResult bjResetForwardList(BjForwardList);
