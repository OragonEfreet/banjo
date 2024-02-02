#include <core/errors.h>
#include <core/memory.h>
#include <engine/app.h>
#include <engine/window.h>

#include <string.h>
#include <unistd.h>

BJAPI_ATTR BjResult BJAPI_CALL bjCreateApplication(
    const BjApplicationCreateInfo*    pCreateInfo,
    BjApplication*                    pApplication
) {
    bjExpectValue(pCreateInfo,  BJ_NULL_CREATE_INFO);
    bjExpectValue(pApplication, BJ_NULL_OUTPUT_HANDLE);


    struct BjApplication_T* application = bjNewStruct(BjApplication, pCreateInfo->pAllocator);

    const char *appName = pCreateInfo->name ? pCreateInfo->name : BJ_NAME;
    application->name = bjNewN(char, strlen(appName)+1, pCreateInfo->pAllocator);
    strcpy(application->name, appName);
    application->shouldClose = false;
    application->pAllocator = pCreateInfo->pAllocator;

    bj_RetainWindowComponent(application);

    *pApplication = application;
    return BJ_SUCCESS;
}

BJAPI_ATTR void BJAPI_CALL bjDestroyApplication(
    BjApplication                     application
){
    bjExpectValue(application, VK_NULL_INPUT_HANDLE);

    bj_ReleaseWindowComponent(application);

    bjFree(application->name, application->pAllocator);
    bjFree(application, application->pAllocator);
}

BJAPI_ATTR bool BJAPI_CALL bjApplicationShouldClose(
    BjApplication application
) {
    return application->shouldClose;
}

BJAPI_ATTR void BJAPI_CALL bjCloseApplication(
    BjApplication application
) {
    bjExpectValue(application, VK_NULL_INPUT_HANDLE);
    application->shouldClose = true;
}

BJAPI_ATTR void BJAPI_CALL bjTickApplication(
    BjApplication application
) {
    static usize fcount = 0;
    bj_ProcessEvents();

    for(usize w = application->windowCount ; w > 0 ; --w) {
        BjWindow window = application->pWindows[w - 1];
        if(bjWindowShouldClose(window)) {
            bjDestroyWindow(window);
            if(application->windowCount == 0) {
                application->shouldClose = true;
            }
        }
    }
}

void bj_LinkWindow(
    BjApplication application,
    BjWindow      window
) {
    if(application->windowCount == 0) {
        application->pWindows = bjNew(BjWindow, application->pAllocator);
    } else {
        application->pWindows = bjReallocate(application->pWindows, application->windowCount+1, application->pAllocator);
    }
    application->pWindows[application->windowCount++] = window;
}

void bj_UnlinkWindow(
    BjApplication application,
    BjWindow      window
) {
    // To remove an element from an array, we "switch" it with latest position
    // and reduce the array size by 1.
    // Switch only has to be done halfway-through since we don't need the removed
    // handle to exist anymore.
    for(size_t w = 0; w < application->windowCount ; ++w) {
        if(application->pWindows[w] == window) {
            if(w != application->windowCount - 1) {
                application->pWindows[w] = application->pWindows[application->windowCount - 1];
            }
            application->pWindows = bjReallocate(application->pWindows, --application->windowCount, application->pAllocator);
            break;
        }
    }
}
