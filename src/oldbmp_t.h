#pragma once

#include <banjo/oldbmp.h>

struct bj_oldbmp_t {
    usize     width;
    usize     height;
    bj_color* buffer;
    bj_color  clear_color;
};

bj_oldbmp* bj_oldbmp_init(bj_oldbmp* p_oldbmp, usize width, usize height);
void bj_oldbmp_reset(bj_oldbmp* p_oldbmp);

