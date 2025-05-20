#pragma once

#include <banjo/rbuffer.h>

struct bj_rbuffer_t {
    size_t bytes_payload;
    size_t capacity;
    size_t head;
    size_t len;
    void*  p_buffer;
};


