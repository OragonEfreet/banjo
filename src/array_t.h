#pragma once

#include <banjo/array.h>

struct bj_array_t {
    size_t bytes_payload;
    size_t len;
    size_t capacity;
    void* p_buffer;
};

bj_array* bj_array_init(bj_array* p_instance, size_t bytes_payload, size_t capacity);
void bj_array_reset(bj_array* p_array);

