#pragma once

#include <banjo/array.h>

struct bj_array_t {
    usize bytes_payload;
    usize len;
    usize capacity;
    void* p_buffer;
};

bj_array* bj_array_init(bj_array* p_instance, usize bytes_payload, usize capacity);
void bj_array_reset(bj_array* p_array);

