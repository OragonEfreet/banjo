#include <banjo/error.h>
#include <data/forward_list.h>

#include <string.h>

/* #include <string.h> */

static const usize entry_ptr_size = sizeof(BjForwardListEntry);

void bjInitForwardList(
    const BjForwardListInfo*     pInfo,
    const BjAllocationCallbacks* pAllocator,
    BjForwardList                pInstance
) {
    bjAssert(pInfo != 0);

    pInstance->pAllocator   = pAllocator;
    pInstance->value_size   = pInfo->value_size;
    pInstance->weak_owning  = pInfo->weak_owning;
    pInstance->pHead = 0;
}

void bjResetForwardList(
    BjForwardList list
) {
    bjAssert(list != 0);
    bjClearForwardList(list);
}

BjForwardList bjCreateForwardList(
    const BjForwardListInfo*     pInfo,
    const BjAllocationCallbacks* pAllocator
) {
    bjAssert(pInfo != 0);
    BjForwardList list = bjNewStruct(BjForwardList, pAllocator);
    bjInitForwardList(pInfo, pAllocator, list);
    return list;
}

void bjClearForwardList(
    BjForwardList list
) {
    bjAssert(list != 0);

    BjForwardListEntry* entry = list->pHead;
    while(entry != 0) {
        BjForwardListEntry* next = entry->pNext;
        if(list->weak_owning == false && list->value_size > entry_ptr_size) {
            bjFree(entry->value, list->pAllocator);
        }
        bjFree(entry, list->pAllocator);
        entry = next;
    }
}

void bjDestroyForwardList(
    BjForwardList list
) {
    bjAssert(list != 0);
    bjResetForwardList(list);
    bjFree(list, list->pAllocator);
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

void bjForwardListInsert(
    BjForwardList list,
    usize index,
    void* pData
) {
    bjAssert(list != 0);

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
        if(list->value_size > entry_ptr_size) {
            new_entry->value = bjAllocate(list->value_size, list->pAllocator);
            memcpy(new_entry->value, pData, list->value_size);
        } else {
            memcpy(&new_entry->value, pData, list->value_size);
        }
    }

    *source_ptr = new_entry;
}

void bjForwardListPrepend(
    BjForwardList list,
    void* pData
) {
    bjForwardListInsert(list, 0, pData);
}

void* bjForwardListValue(
    BjForwardList list,
    usize index
) {
    BjForwardListEntry* entry = list->pHead;
    usize current_index = 0;
    while(entry != 0) {
        if(current_index++ == index) {
            if(list->value_size > entry_ptr_size) {
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

