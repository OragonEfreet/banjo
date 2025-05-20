#pragma once

#include <banjo/rbuffer.h>

struct bj_rbucket_t;

struct bj_rbuffer_t {
    size_t               item_payload;
    size_t               bucket_size;
    size_t               n_buckets;
    struct bj_rbucket_t* p_buckets;
};


