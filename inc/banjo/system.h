#pragma once

#include <banjo/api.h>
#include <banjo/error.h>

BANJO_EXPORT bool bj_system_init(
    bj_error** p_error
);

BANJO_EXPORT void bj_system_dispose(
    bj_error** p_error
);
