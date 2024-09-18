#pragma once

#include <banjo/list.h>

struct bj_list_t {
    size_t  bytes_payload;
    size_t  bytes_entry;
    void*  p_head;
    bool   weak_owning;
};

bj_list* bj_list_init(bj_list* p_list, size_t  bytes_payload);
void bj_list_reset(bj_list* p_list);
