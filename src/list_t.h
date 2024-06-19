#pragma once

#include <banjo/list.h>

struct bj_list_t {
    usize  bytes_payload;
    usize  bytes_entry;
    void*  p_head;
    bool   weak_owning;
};

bj_list* bj_list_init(bj_list* p_list, usize  bytes_payload);
void bj_list_reset(bj_list* p_list);
