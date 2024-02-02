#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

typedef struct BjArray {
    BjAllocationCallbacks* pAllocator;
    void*                  data;
    usize                  capacity;
    usize                  count;
} BjArray;

BJAPI_ATTR BjResult BJAPI_CALL bjInitArray(
    BjArray* array
);

BJAPI_ATTR BjResult BJAPI_CALL bjDropArray(
    BjArray* array
);




