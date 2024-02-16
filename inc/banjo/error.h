#pragma once

#include <banjo/api.h>
#include <banjo/memory.h>

typedef struct BjError_T {
    u32 domain;
    u32 code;
} BjError;

BANJO_EXPORT void bjSetError(
    BjError* pError,
    u32 domain,
    u32 code
);




