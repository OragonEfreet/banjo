#include <errors.h>
#include <memory.h>
#include <_rfu/app.h>
#include <_rfu/window.h>

#include <string.h>
#include <unistd.h>

BjResult bjCreateApplication(
    const BjApplicationInfo*    pInfo,
    BjApplication*                    pApplication
) {
    bjExpectValue(pInfo,  BJ_NULL_CREATE_INFO);
    bjExpectValue(pApplication, BJ_NULL_OUTPUT_HANDLE);


    struct BjApplication_T* application = bjNewStruct(BjApplication, pInfo->pAllocator);

    const char *appName = pInfo->name ? pInfo->name : BJ_NAME;
    application->name = bjNewN(char, strlen(appName)+1, pInfo->pAllocator);
    strcpy(application->name, appName);
    application->shouldClose = false;
    application->pAllocator = pInfo->pAllocator;

    bj_RetainWindowComponent(application);

    *pApplication = application;
    return BJ_SUCCESS;
}

BjResult bjDestroyApplication(
    BjApplication                     application
){
    bjExpectValue(application, BJ_NULL_INPUT_HANDLE);

    bj_ReleaseWindowComponent(application);

    bjFree(application->name, application->pAllocator);
    bjFree(application, application->pAllocator);
    return BJ_SUCCESS;
}

bool bjApplicationShouldClose(
    BjApplication application
) {
    return application->shouldClose;
}

BjResult bjCloseApplication(
    BjApplication application
) {
    bjExpectValue(application, BJ_NULL_INPUT_HANDLE);
    application->shouldClose = true;
    return BJ_SUCCESS;
}

void bjTickApplication(
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
