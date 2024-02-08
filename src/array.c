#include <data/array.h>
#include <errors.h>

BjResult bjInitArray(
    const BjArrayCreateInfo* pCreateInfo,
    BjArray                  pInstance
) {
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);

    pInstance->pAllocator = pCreateInfo->pAllocator;
    pInstance->capacity   = 0;
    pInstance->count      = 0;
    pInstance->elem_size  = pCreateInfo->elem_size;
    pInstance->pData      = 0;

    if(pCreateInfo->capacity > 0) {
        bjReserveArray(pInstance, pCreateInfo->capacity);
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
    const BjArrayCreateInfo* pCreateInfo,
    BjArray* pInstance
) {
    bjExpectValue(pInstance, BJ_NULL_OUTPUT_HANDLE);
    bjExpectValue(pCreateInfo->elem_size, BJ_INVALID_PARAMETER);

    BjArray array     = bjNewStruct(BjArray, pCreateInfo->pAllocator);

    BjResult result = bjInitArray(pCreateInfo, array);
    if(result == BJ_SUCCESS) {
        *pInstance = array;
    } else {
        bjFree(array, pCreateInfo->pAllocator);
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
            pArray->pData = bjAllocate(pArray->elem_size * capacity, pArray->pAllocator);
        } else {
            pArray->pData = bjReallocate(pArray->pData, pArray->elem_size * capacity, pArray->pAllocator);
        }
        pArray->capacity = capacity;
    }
    return BJ_SUCCESS;
}
