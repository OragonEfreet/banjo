#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include "array_t.h"
#include "check.h"

bj_array* bj_array_alloc(
    void
) {
    return bj_malloc(sizeof(bj_array));
}

bj_array* bj_array_new(
    size_t     bytes_payload
) {
    bj_array array;
    if(bj_array_init(&array, bytes_payload) == 0) {
        return 0;
    }
    return bj_memcpy(bj_array_alloc(), &array, sizeof(bj_array));
}

void bj_array_del(
    bj_array* p_array
) {
    bj_array_reset(p_array);
    bj_free(p_array);
}

bj_array* bj_array_init(
    bj_array* p_instance,
    size_t bytes_payload
) {
    bj_memset(p_instance, 0, sizeof(bj_array));
    if(bytes_payload > 0) {
        p_instance->bytes_payload = bytes_payload;
    }
    return p_instance;
}

void bj_array_reset(
    bj_array* array
) {
    bj_check(array != 0);
    bj_free(array->p_buffer);
    bj_memset(array, 0, sizeof(bj_array));
}

void bj_array_clear(
    bj_array* array
) {
    bj_check(array != 0);
    array->len = 0;
}

void bj_array_shrink(
    bj_array* array
) {
    bj_check(array != 0);
    if(array->len < array->capacity) {
        if(array->len == 0) {
            bj_free(array->p_buffer);
            array->p_buffer = 0;
        } else {
            array->p_buffer = bj_realloc(array->p_buffer, array->bytes_payload * array->len);
        }
        array->capacity = array->len;
    }
}

void bj_array_set_len(
    bj_array* array,
    size_t   len
) {
    bj_check(array != 0);
    array->len = array->bytes_payload == 0 ? 0 : len;
    if(array->capacity < len) {
        bj_array_reserve(array, len * 2);
    }
}

void bj_array_reserve(
    bj_array* array,
    size_t   capacity
) {
    bj_check(array != 0);
    const size_t bytes_capacity_req = array->bytes_payload * capacity;
    if(bytes_capacity_req > array->bytes_payload * array->capacity) {
        if(array->p_buffer == 0) {
            array->p_buffer = bj_malloc(bytes_capacity_req);
        } else {
            array->p_buffer = bj_realloc(array->p_buffer, bytes_capacity_req);
        }
        array->capacity = capacity;
    }
}

void bj_array_push(
    bj_array* array,
    const void* value
) {
    bj_check(array != 0);
    bj_check(value != 0);

    // If capacity is not enough, growth by twice the target size
    if(array->len + 1 > array->capacity) {
        bj_array_reserve(array, (array->len + 1) * 2); 
    }
    void* dest = ((char*)array->p_buffer) + array->bytes_payload * array->len;
    if(bj_memcpy(dest, value, array->bytes_payload)) {
        ++array->len;
    }
}

void bj_array_pop(
    bj_array* array
) {
    bj_check(array != 0);
    if(array->len > 0) {
        --array->len;
    }
}

void* bj_array_at(
    const bj_array* array,
    size_t   at
) {
    bj_check_or_0(array);
    if(at < array->len) {
        return ((char*)array->p_buffer) + array->bytes_payload * at;
    }
    return 0;
}

void* bj_array_data(
    const bj_array* array
) {
    bj_check_or_0(array);
    return array->p_buffer;
}

size_t bj_array_len(
    const bj_array* array
) {
    bj_check_or_0(array);
    return array->len;
}

size_t bj_array_capacity(
    const bj_array* array
) {
    bj_check_or_0(array);
    return array->capacity;
}

