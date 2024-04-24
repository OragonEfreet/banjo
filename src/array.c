#include "banjo/memory.h"
#include <banjo/array.h>
#include <banjo/error.h>

#include "obj.h"

BJ_IMPL_OBJ(Array, array)

void bj_array_init(
    BjArray*           p_instance,
    const BjArrayInfo* p_info
) {
    bj_memset(p_instance, 0, sizeof(BjArray));
    if(p_info != 0 && p_info->bytes_payload > 0) {
        p_instance->info.bytes_payload = p_info->bytes_payload;
        bj_array_reserve(p_instance, p_info->len);
        bj_array_set_len(p_instance, p_info->len);
        bj_array_reserve(p_instance, p_info->capacity);
    }
}

void bj_array_reset(
    BjArray* array
) {
    bj_assert(array != 0);
    bj_free(array->p_buffer, array->info.p_allocator);
    bj_memset(array, 0, sizeof(BjArray));
}

void bj_array_clear(
    BjArray* array
) {
    bj_assert(array != 0);
    array->info.len = 0;
}

void bj_array_shrink(
    BjArray* array
) {
    bj_assert(array != 0);
    if(array->info.len < array->info.capacity) {
        if(array->info.len == 0) {
            bj_free(array->p_buffer, array->info.p_allocator);
            array->p_buffer = 0;
        } else {
            array->p_buffer = bj_realloc(array->p_buffer, array->info.bytes_payload * array->info.len, array->info.p_allocator);
        }
        array->info.capacity = array->info.len;
    }
}

void bj_array_set_len(
    BjArray* array,
    usize   len
) {
    bj_assert(array != 0);
    array->info.len = array->info.bytes_payload == 0 ? 0 : len;
    if(array->info.capacity < len) {
        bj_array_reserve(array, len * 2);
    }
}

void bj_array_reserve(
    BjArray* array,
    usize   capacity
) {
    bj_assert(array != 0);
    const usize bytes_capacity_req = array->info.bytes_payload * capacity;
    if(bytes_capacity_req > array->info.bytes_payload * array->info.capacity) {
        if(array->p_buffer == 0) {
            array->p_buffer = bj_malloc(bytes_capacity_req, array->info.p_allocator);
        } else {
            array->p_buffer = bj_realloc(array->p_buffer, bytes_capacity_req, array->info.p_allocator);
        }
        array->info.capacity = capacity;
    }
}

void bj_array_push(
    BjArray* array,
    const void* value
) {
    bj_assert(array != 0);
    bj_assert(value != 0);

    // If capacity is not enough, growth by twice the target size
    if(array->info.len + 1 > array->info.capacity) {
        bj_array_reserve(array, (array->info.len + 1) * 2); 
    }
    void* dest = ((byte*)array->p_buffer) + array->info.bytes_payload * array->info.len;
    if(bj_memcpy(dest, value, array->info.bytes_payload)) {
        ++array->info.len;
    }
}

void bj_array_pop(
    BjArray* array
) {
    bj_assert(array != 0);
    if(array->info.len > 0) {
        --array->info.len;
    }
}

void* bj_array_at(
    const BjArray* array,
    usize   at
) {
    bj_assert(array);
    if(at < array->info.len) {
        return ((byte*)array->p_buffer) + array->info.bytes_payload * at;
    }
    return 0;
}

void* bj_array_data(
    const BjArray* array
) {
    bj_assert(array);
    return array->p_buffer;
}

usize bj_array_len(
    const BjArray* array
) {
    bj_assert(array);
    return array->info.len;
}

usize bj_array_capacity(
    const BjArray* array
) {
    bj_assert(array);
    return array->info.capacity;
}

