#pragma once

#include <banjo/htable.h>

#include "array_t.h"

struct bj_htable_t {
    size_t             bytes_value;
    size_t             bytes_key;
    size_t             bytes_entry;
    bj_hash_fn        fn_hash;
    bj_array          buckets;
    bj_bool              weak_owning;
};

