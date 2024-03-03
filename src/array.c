#include "banjo/memory.h"
#include <banjo/array.h>
#include <banjo/error.h>

void bj_array_init(
    BjArray*                       p_instance,
    const BjArrayInfo*             p_info,
    const BjAllocationCallbacks*   p_allocator
) {
    bj_memset(p_instance, 0, sizeof(BjArray));
    if(p_info != 0 && p_info->bytes_payload > 0) {
        p_instance->p_allocator   = p_allocator;
        p_instance->bytes_payload = p_info->bytes_payload;

        bj_array_reserve(p_instance, p_info->count);
        bj_array_set_count(p_instance, p_info->count);
        bj_array_reserve(p_instance, p_info->capacity);
    }
}

void bj_array_reset(
    BjArray* array
) {
    bj_assert(array != 0);
    bj_free(array->p_buffer, array->p_allocator);
    bj_memset(array, 0, sizeof(BjArray));
}

BANJO_EXPORT void bj_array_clear(
    BjArray* array
) {
    bj_assert(array != 0);
    array->count = 0;
}

BANJO_EXPORT void bj_array_shrink(
    BjArray* array
) {
    bj_assert(array != 0);
    if(array->count < array->capacity) {
        if(array->count == 0) {
            bj_free(array->p_buffer, array->p_allocator);
            array->p_buffer = 0;
        } else {
            array->p_buffer = bj_realloc(array->p_buffer, array->bytes_payload * array->count, array->p_allocator);
        }
        array->capacity = array->count;
    }
}

void bj_array_set_count(
    BjArray* array,
    usize   count
) {
    bj_assert(array != 0);
    array->count = array->bytes_payload == 0 ? 0 : count;
    if(array->capacity < count) {
        bj_array_reserve(array, count * 2);
    }
}

void bj_array_reserve(
    BjArray* array,
    usize   capacity
) {
    bj_assert(array != 0);
    const usize bytes_capacity_req = array->bytes_payload * capacity;
    if(bytes_capacity_req > array->bytes_payload * array->capacity) {
        if(array->p_buffer == 0) {
            array->p_buffer = bj_malloc(bytes_capacity_req, array->p_allocator);
        } else {
            array->p_buffer = bj_realloc(array->p_buffer, bytes_capacity_req, array->p_allocator);
        }
        array->capacity = capacity;
    }
}

BANJO_EXPORT void bj_array_push(
    BjArray* array,
    const void* value
) {
    bj_assert(array != 0);
    bj_assert(value != 0);

    // Request for at least twice the new size
    bj_array_reserve(array, (array->count + 1) * 2); 
    void* dest = ((byte*)array->p_buffer) + array->bytes_payload * array->count;
    if(bj_memcpy(dest, value, array->bytes_payload)) {
        ++array->count;
    }
}

BANJO_EXPORT void bj_array_pop(
    BjArray* array
) {
    bj_assert(array != 0);
    if(array->count > 0) {
        --array->count;
    }
}

void* bj_array_at(
    const BjArray* array,
    usize   at
) {
    bj_assert(array);
    if(at < array->count) {
        return ((byte*)array->p_buffer) + array->bytes_payload * at;
    }
    return 0;
}

BANJO_EXPORT void* bj_array_data(
    const BjArray* array
) {
    bj_assert(array);
    return array->p_buffer;
}

usize bj_array_count(
    const BjArray* array
) {
    bj_assert(array);
    return array->count;
}

