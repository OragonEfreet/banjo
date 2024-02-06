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

    BjArray array = bjNew(BjArray, pCreateInfo->pAllocator);
    array->pAllocator = pCreateInfo->pAllocator;
    array->count = 0;
    array->elem_size = pCreateInfo->elem_size;
    array->data = 0;

    *pInstance = array;

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
