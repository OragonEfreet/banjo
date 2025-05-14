#pragma once

#include <banjo/list.h>

struct bj_list_t {
    size_t  bytes_payload;
    size_t  bytes_entry;
    void*  p_head;
    bj_bool   weak_owning;
};

