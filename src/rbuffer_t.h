#pragma once

#include <banjo/rbuffer.h>

struct bj_rbucket_t;

struct bj_rbuffer_t {
    // size_t               bucket_size;
    // size_t               n_buckets;
    // struct bj_rbucket_t* buckets;

    size_t head;
    size_t tail;
    size_t _capacity;
};


