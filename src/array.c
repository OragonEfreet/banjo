#include <data/array.h>
#include <banjo/error.h>

void p_array_init(
    const BjArrayInfo*           p_info,
    const BjAllocationCallbacks* p_allocator,
    BjArray                      p_instance
) {
    bj_assert(p_info != 0);

    p_instance->p_allocator = p_allocator;
    p_instance->capacity    = 0;
    p_instance->count       = 0;
    p_instance->value_size  = p_info->value_size;
    p_instance->p_data      = 0;

    if(p_info->capacity > 0) {
        bj_array_reserve(p_instance, p_info->capacity);
    }
}

void p_array_reset(
    BjArray array
) {
    bj_assert(array != 0);
    bj_free(array->p_data, array->p_allocator);
}

BjArray bj_array_create(
    const BjArrayInfo*     p_info,
    const BjAllocationCallbacks* p_allocator
) {
    bj_assert(p_info != 0);
    BjArray array     = bj_new_struct(BjArray, p_allocator);
    p_array_init(p_info, p_allocator, array);
    return array;
}

void bj_array_destroy(
    BjArray array
) {
    bj_assert(array != 0);
    p_array_reset(array);
    bj_free(array, array->p_allocator);
}

void bj_array_reserve(
    BjArray p_array,
    usize   capacity
) {
    bj_assert(p_array != 0);
    if(capacity > p_array->capacity) {
        if(p_array->p_data == 0) {
            p_array->p_data = bj_malloc(p_array->value_size * capacity, p_array->p_allocator);
        } else {
            p_array->p_data = bj_realloc(p_array->p_data, p_array->value_size * capacity, p_array->p_allocator);
        }
        p_array->capacity = capacity;
    }
}
