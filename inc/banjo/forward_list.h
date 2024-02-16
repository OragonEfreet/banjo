#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjForwardList);

typedef struct BjForwardListInfo {
    usize                  value_size;
    bool                   weak_owning;
} BjForwardListInfo;

BANJO_EXPORT BjForwardList bj_forward_list_create(
    const BjForwardListInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
);

BANJO_EXPORT void bj_forward_list_destroy(
    BjForwardList list
);

BANJO_EXPORT void bj_forward_list_clear(
    BjForwardList list
);

BANJO_EXPORT usize bj_forward_list_count(
    BjForwardList list
);

BANJO_EXPORT void bj_forward_list_insert(
    BjForwardList list,
    usize index,
    void* pData
);

BANJO_EXPORT void bj_forward_list_prepend(
    BjForwardList list,
    void* pData
);

BANJO_EXPORT void* bj_forward_list_value(
    BjForwardList list,
    usize index
);

BANJO_EXPORT void* bj_forward_list_head(
    BjForwardList list
);

typedef int (*PFN_bjValueCmp)(
    const void* p_left,
    const void* p_right,
    usize       mem_size
);

BANJO_EXPORT void* bj_forward_list_find(
    BjForwardList  list,
    void*          value,
    PFN_bjValueCmp fn_cmp
);





