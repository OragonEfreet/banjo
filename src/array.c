#include <data/array.h>
#include <banjo/error.h>

void bjInitArray(
    const BjArrayInfo*           pInfo,
    const BjAllocationCallbacks* pAllocator,
    BjArray                      pInstance
) {
    bjAssert(pInfo != 0);

    pInstance->pAllocator = pAllocator;
    pInstance->capacity   = 0;
    pInstance->count      = 0;
    pInstance->value_size  = pInfo->value_size;
    pInstance->pData      = 0;

    if(pInfo->capacity > 0) {
        bjReserveArray(pInstance, pInfo->capacity);
    }
}

void bjResetArray(
    BjArray array
) {
    bjAssert(array != 0);
    bjFree(array->pData, array->pAllocator);
}

BjArray bjCreateArray(
    const BjArrayInfo*     pInfo,
    const BjAllocationCallbacks* pAllocator
) {
    bjAssert(pInfo != 0);
    BjArray array     = bjNewStruct(BjArray, pAllocator);
    bjInitArray(pInfo, pAllocator, array);
    return array;
}

void bjDestroyArray(
    BjArray array
) {
    bjAssert(array != 0);
    bjResetArray(array);
    bjFree(array, array->pAllocator);
}

void bjReserveArray(
    BjArray pArray,
    usize   capacity
) {
    bjAssert(pArray != 0);
    if(capacity > pArray->capacity) {
        if(pArray->pData == 0) {
            pArray->pData = bjAllocate(pArray->value_size * capacity, pArray->pAllocator);
        } else {
            pArray->pData = bjReallocate(pArray->pData, pArray->value_size * capacity, pArray->pAllocator);
        }
        pArray->capacity = capacity;
    }
}
