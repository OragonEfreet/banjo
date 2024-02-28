#include <banjo/array.h>
#include <banjo/error.h>


BjArray* bj_array_create(
    const BjArrayInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
) {
    bj_assert(p_info != 0);
    BjArray* array = bj_malloc(sizeof(struct BjArray_T), p_allocator);
    bj_array_init(p_info, p_allocator, array);
    return array;
}

void bj_array_destroy(
    BjArray* array
) {
    bj_assert(array != 0);
    bj_array_reset(array);
    bj_free(array, array->p_allocator);
}

void bj_array_init(
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator,
    BjArray*                     p_instance
) {
    bj_assert(p_info != 0);

    bj_array_reset(p_instance);

    p_instance->p_allocator = p_allocator;
    p_instance->value_size  = p_info->value_size;
    p_instance->capacity    = 0;
    p_instance->count       = 0;
    p_instance->p_data      = 0;

    bj_array_reserve(p_instance, p_info->count);
    bj_array_set_count(p_instance, p_info->count);
    bj_array_reserve(p_instance, p_info->capacity);

}

void bj_array_reset(
    BjArray* array
) {
    bj_assert(array != 0);
    bj_free(array->p_data, array->p_allocator);

    array->p_allocator = 0;
    array->value_size  = 0;
    array->capacity    = 0;
    array->count       = 0;
    array->p_data      = 0;
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
            bj_free(array->p_data, array->p_allocator);
            array->p_data = 0;
        } else {
            array->p_data = bj_realloc(array->p_data, array->value_size * array->count, array->p_allocator);
        }
        array->capacity = array->count;
    }
}

void bj_array_set_count(
    BjArray* array,
    usize   count
) {
    bj_assert(array != 0);
    if(count == array->count) {
        return;
    }

    if(array->capacity < count) {
        bj_array_reserve(array, count * 2);
    }
    array->count = count;
}

void bj_array_reserve(
    BjArray* array,
    usize   capacity
) {
    bj_assert(array != 0);
    if(capacity > array->capacity) {
        if(array->p_data == 0) {
            array->p_data = bj_malloc(array->value_size * capacity, array->p_allocator);
        } else {
            array->p_data = bj_realloc(array->p_data, array->value_size * capacity, array->p_allocator);
        }
#ifdef BANJO_PEDANTIC
    if(capacity > array->capacity) {
        bj_memset((char*)array->p_data + capacity, 0, capacity - array->capacity);
    }
#endif
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
    void* dest = ((byte*)array->p_data) + array->value_size * array->count;
    bj_memcpy(dest, value, array->value_size);
    ++array->count;
}

BANJO_EXPORT void bj_array_pop(
    BjArray* array
) {
    bj_assert(array != 0);
    bj_assert(array->count > 0);
    --array->count;
}

void* bj_array_at(
    const BjArray* array,
    usize   at
) {
    bj_assert(array);
    bj_assert(at < array->count);
    return ((byte*)array->p_data) + array->value_size * at;
}

BANJO_EXPORT void* bj_array_data(
    const BjArray* array
) {
    bj_assert(array);
    return array->p_data;
}

usize bj_array_count(
    const BjArray* array
) {
    bj_assert(array);
    return array->count;
}

