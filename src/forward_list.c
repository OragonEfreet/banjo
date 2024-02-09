#include <data/forward_list.h>
#include <errors.h>

/* #include <string.h> */

BjResult bjInitForwardList(
    const BjForwardListCreateInfo* pCreateInfo,
    BjForwardList                  pInstance
) {
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);

    pInstance->pAllocator  = pCreateInfo->pAllocator;
    pInstance->elem_size   = pCreateInfo->elem_size;
    pInstance->pFirstEntry = 0;

    return BJ_SUCCESS;
}

BjResult bjResetForwardList(
    BjForwardList list
) {
    bjExpectValue(list, BJ_NULL_PARAMETER);
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
        BjForwardListEntry* entry = list->pFirstEntry;
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

    BjForwardListEntry** source_ptr = &list->pFirstEntry;
    for(usize i = 0 ; (i < index) && ((*source_ptr) != 0) ; ++i) {
        source_ptr = &(*source_ptr)->pNext;
    }
    BjForwardListEntry* next_entry = *source_ptr ? (*source_ptr) : 0;

    BjForwardListEntry* new_entry = bjAllocate(sizeof(BjForwardListEntry), list->pAllocator);
    new_entry->pNext = next_entry;
    new_entry->value = 0; // TODO
    /* entry->value = bjAllocate(list->elem_size, list->pAllocator); */
    /* memcpy(entry->value, pData, list->elem_size); */

    *source_ptr = new_entry;
    return BJ_SUCCESS;
}

BjResult bjForwardListPrepend(
    BjForwardList list,
    void* pData
) {
    return bjForwardListInsert(list, 0, pData);
}

