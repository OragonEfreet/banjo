#include <banjo/array.h>
#include <core/errors.h>
#include <core/memory.h>

BjResult bjCreateArray(
    const BjArrayCreateInfo* pCreateInfo,
    BjArray* pInstance
) {
    bjExpectValue(pInstance, BJ_NULL_OUTPUT_HANDLE);
    bjExpectValue(pCreateInfo, BJ_NULL_CREATE_INFO);
    bjExpectValue(pCreateInfo->elem_size, BJ_INVALID_PARAMETER);

    BjArray array     = bjNewStruct(BjArray, pCreateInfo->pAllocator);
    array->pAllocator = pCreateInfo->pAllocator;
    array->capacity   = 0;
    array->count      = 0;
    array->elem_size  = pCreateInfo->elem_size;
    array->data       = 0;

    if(pCreateInfo->capacity > 0) {
        bjReserveArray(array, pCreateInfo->capacity);
    }

    *pInstance = array;

    return BJ_SUCCESS;
}

BjResult bjReserveArray(
    BjArray pArray,
    usize   capacity
) {
    bjExpectValue(pArray, BJ_NULL_OUTPUT_HANDLE);
    if(capacity > pArray->capacity) {
        if(pArray->data == 0) {
            pArray->data = bjAllocate(pArray->elem_size * capacity, pArray->pAllocator);
        } else {
            pArray->data = bjReallocate(pArray->data, pArray->elem_size * capacity, pArray->pAllocator);
        }
        pArray->capacity = capacity;
    }
    return BJ_SUCCESS;
}

BjResult bjDestroyArray(
    BjArray array
) {
    bjExpectValue(array, BJ_NULL_PARAMETER);
    bjFree(array->data, array->pAllocator);
    bjFree(array, array->pAllocator);
    return BJ_SUCCESS;
}
