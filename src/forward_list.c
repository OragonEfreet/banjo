#include <data/forward_list.h>
#include <errors.h>
#include <string.h>

/* #include <string.h> */

static const usize entry_ptr_size = sizeof(BjForwardListEntry);

BjResult bjInitForwardList(
    const BjForwardListCreateInfo* pCreateInfo,
    BjForwardList                  pInstance
) {
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);

    pInstance->pAllocator  = pCreateInfo->pAllocator;
    pInstance->elem_size   = pCreateInfo->elem_size;
    pInstance->weak_owning = pCreateInfo->weak_owning;
    pInstance->pHead = 0;

    return BJ_SUCCESS;
}

BjResult bjResetForwardList(
    BjForwardList list
) {
    bjExpectValue(list, BJ_NULL_PARAMETER);
    bjClearForwardList(list);
    return BJ_SUCCESS;
}

BjResult bjCreateForwardList(
    const BjForwardListCreateInfo* pCreateInfo,
    BjForwardList* pInstance
) {
    bjExpectValue(pInstance, BJ_NULL_OUTPUT_HANDLE);
    bjExpectValue(pCreateInfo->elem_size, BJ_INVALID_PARAMETER);

    BjForwardList list     = bjNewStruct(BjForwardList, pCreateInfo->pAllocator);

    BjResult result = bjInitForwardList(pCreateInfo, list);
    if(result == BJ_SUCCESS) {
        *pInstance = list;
    } else {
        bjFree(list, pCreateInfo->pAllocator);
    }

    return BJ_SUCCESS;
}

BjResult bjClearForwardList(
    BjForwardList list
) {
    bjExpectValue(list, BJ_NULL_INPUT_HANDLE);

    BjForwardListEntry* entry = list->pHead;
    while(entry != 0) {
        BjForwardListEntry* next = entry->pNext;
        if(list->weak_owning == false && list->elem_size > entry_ptr_size) {
            bjFree(entry->value, list->pAllocator);
        }
        bjFree(entry, list->pAllocator);
        entry = next;
    }

    return BJ_SUCCESS;
}

BjResult bjDestroyForwardList(
    BjForwardList list
) {
    bjExpectValue(list, BJ_NULL_PARAMETER);
    BjResult res = bjResetForwardList(list);
    bjFree(list, list->pAllocator);
    return res;
}

usize bjForwardListCount(
    BjForwardList list
) {
    usize result = 0;
    if(list != 0) {
        BjForwardListEntry* entry = list->pHead;
        while(entry != 0) {
            result += 1;
            entry = entry->pNext;
        }
    }
    return result;
}

BjResult bjForwardListInsert(
    BjForwardList list,
    usize index,
    void* pData
) {
    bjExpectValue(list, BJ_NULL_PARAMETER);

    BjForwardListEntry** source_ptr = &list->pHead;
    for(usize i = 0 ; (i < index) && ((*source_ptr) != 0) ; ++i) {
        source_ptr = &(*source_ptr)->pNext;
    }
    BjForwardListEntry* next_entry = *source_ptr ? (*source_ptr) : 0;

    BjForwardListEntry* new_entry = bjAllocate(sizeof(BjForwardListEntry), list->pAllocator);
    new_entry->pNext = next_entry;
    if(list->weak_owning) {
        new_entry->value = pData;
    } else {
        if(list->elem_size > entry_ptr_size) {
            new_entry->value = bjAllocate(list->elem_size, list->pAllocator);
            memcpy(new_entry->value, pData, list->elem_size);
        } else {
            memcpy(&new_entry->value, pData, list->elem_size);
        }
    }

    *source_ptr = new_entry;
    return BJ_SUCCESS;
}

BjResult bjForwardListPrepend(
    BjForwardList list,
    void* pData
) {
    return bjForwardListInsert(list, 0, pData);
}

void* bjForwardListValue(
    BjForwardList list,
    usize index
) {
    BjForwardListEntry* entry = list->pHead;
    usize current_index = 0;
    while(entry != 0) {
        if(current_index++ == index) {
            if(list->elem_size > entry_ptr_size) {
                return entry->value;
            } else {
                return &entry->value;
            }
            
        }
        entry = entry->pNext;
    }

    return 0;
}

void* bjForwardListHead(
    BjForwardList list
){
    return bjForwardListValue(list, 0);
}

