#pragma once

#include <banjo/core.h>
#include <banjo/memory.h>

BJ_DEFINE_HANDLE(BjApplication);

typedef struct {
    const char*            name;
    BjAllocationCallbacks* pAllocator;
} BjApplicationCreateInfo;

BJAPI_ATTR BjResult BJAPI_CALL bjCreateApplication(
    const BjApplicationCreateInfo* pCreateInfo,
    BjApplication*                 pApplication
);

BJAPI_ATTR void BJAPI_CALL bjDestroyApplication(
    BjApplication application
);

BJAPI_ATTR bool BJAPI_CALL bjApplicationShouldClose(
    BjApplication application
);

BJAPI_ATTR void BJAPI_CALL bjCloseApplication(
    BjApplication application
);

BJAPI_ATTR void BJAPI_CALL bjTickApplication(
    BjApplication application
);



    

