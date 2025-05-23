#pragma once

#include <banjo/rbuffer.h>

struct bj_rbuffer_t {
    size_t write;
    size_t read;
    size_t capacity;
};

BANJO_EXPORT bj_rbuffer* bj_rbuffer_init( bj_rbuffer* p_instance, size_t capacity);
BANJO_EXPORT void bj_rbuffer_reset(bj_rbuffer* p_rbuffer);



