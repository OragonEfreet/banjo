#pragma once

#include "banjo/memory.h"
#include <banjo/list.h>

typedef struct BjList_T {
    const BjAllocationCallbacks*     p_allocator;
    usize                            value_size;
    usize                            entry_size;
    bool                             weak_owning;
    void*                            p_head;
} BjList_T;

void bj_list_init(const BjListInfo*, const BjAllocationCallbacks*, BjList);
void bj_list_reset(BjList);

typedef struct BjListIterator_T {
    BjList          list;
    void**          p_current;
} BjListIterator_T;

void bj_list_iterator_init(const BjList, BjListIterator);
void bj_list_iterator_reset(BjListIterator);


