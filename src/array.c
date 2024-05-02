#include <banjo/array.h>
#include <banjo/error.h>
#include <banjo/memory.h>

bj_array* bj_array_init_default(
    bj_array* p_instance,
    usize bytes_payload
) {
    bj_memset(p_instance, 0, sizeof(bj_array));
    if(bytes_payload > 0) {
        p_instance->bytes_payload = bytes_payload;
    }
    return p_instance;
}

bj_array* bj_array_init_with_capacity(
    bj_array* p_instance,
    usize bytes_payload,
    usize capacity
) {
    bj_memset(p_instance, 0, sizeof(bj_array));
    if(bytes_payload > 0) {
        p_instance->bytes_payload = bytes_payload;
        bj_array_reserve(p_instance, capacity);
    }
    return p_instance;
}

bj_array* bj_array_init_with_len(
    bj_array* p_instance,
    usize bytes_payload,
    usize len
) {
    bj_memset(p_instance, 0, sizeof(bj_array));
    if(bytes_payload > 0) {
        p_instance->bytes_payload = bytes_payload;
        bj_array_set_len(p_instance, len);
    }
    return p_instance;
}

bj_array* bj_array_reset(
    bj_array* array
) {
    bj_assert(array != 0);
    bj_free(array->p_buffer);
    bj_memset(array, 0, sizeof(bj_array));
    return array;
}

void bj_array_clear(
    bj_array* array
) {
    bj_assert(array != 0);
    array->len = 0;
}

void bj_array_shrink(
    bj_array* array
) {
    bj_assert(array != 0);
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
    usize   len
) {
    bj_assert(array != 0);
    array->len = array->bytes_payload == 0 ? 0 : len;
    if(array->capacity < len) {
        bj_array_reserve(array, len * 2);
    }
}

void bj_array_reserve(
    bj_array* array,
    usize   capacity
) {
    bj_assert(array != 0);
    const usize bytes_capacity_req = array->bytes_payload * capacity;
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
    bj_assert(array != 0);
    bj_assert(value != 0);

    // If capacity is not enough, growth by twice the target size
    if(array->len + 1 > array->capacity) {
        bj_array_reserve(array, (array->len + 1) * 2); 
    }
    void* dest = ((byte*)array->p_buffer) + array->bytes_payload * array->len;
    if(bj_memcpy(dest, value, array->bytes_payload)) {
        ++array->len;
    }
}

void bj_array_pop(
    bj_array* array
) {
    bj_assert(array != 0);
    if(array->len > 0) {
        --array->len;
    }
}

void* bj_array_at(
    const bj_array* array,
    usize   at
) {
    bj_assert(array);
    if(at < array->len) {
        return ((byte*)array->p_buffer) + array->bytes_payload * at;
    }
    return 0;
}

void* bj_array_data(
    const bj_array* array
) {
    bj_assert(array);
    return array->p_buffer;
}

usize bj_array_len(
    const bj_array* array
) {
    bj_assert(array);
    return array->len;
}

usize bj_array_capacity(
    const bj_array* array
) {
    bj_assert(array);
    return array->capacity;
}

