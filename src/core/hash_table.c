#include <banjo/hash_table.h>
#include <core/errors.h>
#include <core/memory.h>

BjResult bjCreateHashTable(
    const BjHashTableCreateInfo* pCreateInfo,
    BjHashTable* pInstance
) {
    bjExpectValue(pInstance, BJ_NULL_OUTPUT_HANDLE);
    bjExpectValue(pCreateInfo->elem_size, BJ_INVALID_PARAMETER);

    BjHashTable htable = bjNewStruct(BjHashTable, pCreateInfo->pAllocator);

    BjResult result = bjInitHashTable(pCreateInfo, htable);
    if(result == BJ_SUCCESS) {
        *pInstance = htable;
    } else {
        bjFree(htable, pCreateInfo->pAllocator);
    }

    return BJ_SUCCESS;
}

BjResult bjInitHashTable(
    const BjHashTableCreateInfo* pCreateInfo,
    BjHashTable                  pInstance
) {
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);

    pInstance->pAllocator = pCreateInfo->pAllocator;
    pInstance->elem_size  = pCreateInfo->elem_size;
    // TODO
    bjInitArray(
        &(BjArrayCreateInfo){
            .capacity = 10,
            .elem_size = 1,
            .pAllocator = pCreateInfo->pAllocator,
        },
        &pInstance->buckets
    );

    return BJ_SUCCESS;
}

BjResult bjResetHashTable(
    BjHashTable htable
) {
    BjResult res      = bjClearHashTable(htable);
    bjResetArray(&htable->buckets);
    htable->elem_size = 0;
    return res;
}

BjResult bjDestroyHashTable(
    BjHashTable htable
) {
    bjExpectValue(htable, BJ_NULL_PARAMETER);
    BjResult res =  bjResetHashTable(htable);
    bjFree(htable, htable->pAllocator);
    return res;
}

BANJO_EXPORT BjResult bjClearHashTable(
    BjHashTable htable
) {
    bjExpectValue(htable, BJ_NULL_PARAMETER);

    return BJ_SUCCESS;
}

