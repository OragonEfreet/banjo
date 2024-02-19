#pragma once

#include "banjo/memory.h"
#include <banjo/forward_list.h>

typedef struct BjForwardListEntry {
    void*                        value;
    struct BjForwardListEntry* p_next;
} BjForwardListEntry;

typedef struct BjForwardList_T {
    const BjAllocationCallbacks*     p_allocator;
    usize                            value_size;
    bool                             weak_owning;
    struct BjForwardListEntry*       p_head;
} BjForwardList_T;

void bj_forward_list_init(const BjForwardListInfo*, const BjAllocationCallbacks*, BjForwardList);
void bj_forward_list_reset(BjForwardList);

typedef struct BjForwardListIterator_T {
    BjForwardList                list;
    struct BjForwardListEntry*   p_current;
} BjForwardListIterator_T;

void bj_forward_list_iterator_init(const BjForwardList, BjForwardListIterator);
void bj_forward_list_iterator_reset(BjForwardListIterator);


