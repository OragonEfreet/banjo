#pragma once

#include <banjo/htable.h>

#include "array_t.h"

struct bj_htable_t {
    usize             bytes_value;
    usize             bytes_key;
    usize             bytes_entry;
    bj_hash_fn        fn_hash;
    bj_array          buckets;
    bool              weak_owning;
};

