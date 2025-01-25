#pragma once

#include <banjo/array.h>

struct bj_array_t {
    size_t bytes_payload;
    size_t len;
    size_t capacity;
    void* p_buffer;
};


