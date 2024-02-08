#include <data/forward_list.h>
#include <errors.h>

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
    BjForwardListEntry* entry = list->pFirstEntry;
    while(entry != 0) {
        result += 1;
        entry = entry->pNext;
    }
    return result;
}

usize bjForwardListAppend(
    BjForwardList list,
    void* pData
) {

    


    
    return 0;
}

