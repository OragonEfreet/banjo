#include <core/errors.h>
#include <core/array.h>

BjResult bjInitArray(
    BjArray* array
) {
    bjExpectValue(array, BJ_NULL_PARAMETER);
    bjExpectNull(array->data, BJ_INVALID_PARAMETER);

    if(array->capacity < array->count) {
        array->capacity = array->count;
    }

    if(array->capacity > 0) {
        array->data = bjAllocate(array->capacity, array->pAllocator); 
    }

    return BJ_SUCCESS;
}

BjResult bjDropArray(
    BjArray* array
) {
    bjExpectValue(array, BJ_NULL_PARAMETER);
    bjFree(array->data, array->pAllocator);
    return BJ_SUCCESS;
}
