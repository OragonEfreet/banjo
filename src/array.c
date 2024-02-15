#include <data/array.h>
#include <errors.h>

BjResult bjInitArray(
    const BjArrayInfo* pInfo,
    BjArray                  pInstance
) {
    bjExpectValue(pInfo, BJ_NULL_CREATE_INFO);

    pInstance->pAllocator = pInfo->pAllocator;
    pInstance->capacity   = 0;
    pInstance->count      = 0;
    pInstance->value_size  = pInfo->value_size;
    pInstance->pData      = 0;

    if(pInfo->capacity > 0) {
        bjReserveArray(pInstance, pInfo->capacity);
    }

    return BJ_SUCCESS;
}

BjResult bjResetArray(
    BjArray array
) {
    bjExpectValue(array, BJ_NULL_PARAMETER);
    bjFree(array->pData, array->pAllocator);
    return BJ_SUCCESS;
}

BjResult bjCreateArray(
    const BjArrayInfo* pInfo,
    BjArray* pInstance
) {
    bjExpectValue(pInstance, BJ_NULL_OUTPUT_HANDLE);
    bjExpectValue(pInfo->value_size, BJ_INVALID_PARAMETER);

    BjArray array     = bjNewStruct(BjArray, pInfo->pAllocator);

    BjResult result = bjInitArray(pInfo, array);
    if(result == BJ_SUCCESS) {
        *pInstance = array;
    } else {
        bjFree(array, pInfo->pAllocator);
    }

    return BJ_SUCCESS;
}

BjResult bjDestroyArray(
    BjArray array
) {
    bjExpectValue(array, BJ_NULL_PARAMETER);
    BjResult res = bjResetArray(array);
    bjFree(array, array->pAllocator);
    return res;
}

BjResult bjReserveArray(
    BjArray pArray,
    usize   capacity
) {
    bjExpectValue(pArray, BJ_NULL_OUTPUT_HANDLE);
    if(capacity > pArray->capacity) {
        if(pArray->pData == 0) {
            pArray->pData = bjAllocate(pArray->value_size * capacity, pArray->pAllocator);
        } else {
            pArray->pData = bjReallocate(pArray->pData, pArray->value_size * capacity, pArray->pAllocator);
        }
        pArray->capacity = capacity;
    }
    return BJ_SUCCESS;
}
