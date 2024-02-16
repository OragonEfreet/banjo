#include <_rfu/app.h>
#include <_rfu/window.h>
#include <banjo/error.h>
#include <memory.h>

#include <string.h>
#include <unistd.h>

BjResult bj_application_create(
    const BjApplicationInfo*    p_info,
    BjApplication*                    pApplication
) {
    bjAssert(p_info != 0);
    bjAssert(pApplication != 0);


    struct BjApplication_T* application = bj_new_struct(BjApplication, p_info->p_allocator);

    const char *appName = p_info->name ? p_info->name : BJ_NAME;
    application->name = bj_new_n(char, strlen(appName)+1, p_info->p_allocator);
    strcpy(application->name, appName);
    application->shouldClose = false;
    application->p_allocator = p_info->p_allocator;

    bj_window_retain(application);

    *pApplication = application;
    return BJ_SUCCESS;
}

BjResult bj_application_destroy(
    BjApplication                     application
){
    bjAssert(application != 0);

    bj_window_release(application);

    bj_free(application->name, application->p_allocator);
    bj_free(application, application->p_allocator);
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
    bjAssert(application != 0);
    application->shouldClose = true;
    return BJ_SUCCESS;
}

void bjTickApplication(
    BjApplication application
) {
    static usize fcount = 0;
    bj_ProcessEvents();

    for(usize w = application->windowCount ; w > 0 ; --w) {
        BjWindow window = application->p_windows[w - 1];
        if(bj_window_should_close(window)) {
            bj_window_destroy(window);
            if(application->windowCount == 0) {
                application->shouldClose = true;
            }
        }
    }
}

void bj_window_link(
    BjApplication application,
    BjWindow      window
) {
    if(application->windowCount == 0) {
        application->p_windows = bj_new(BjWindow, application->p_allocator);
    } else {
        application->p_windows = bj_realloc(application->p_windows, application->windowCount+1, application->p_allocator);
    }
    application->p_windows[application->windowCount++] = window;
}

void bj_window_unlink(
    BjApplication application,
    BjWindow      window
) {
    // To remove an element from an array, we "switch" it with latest position
    // and reduce the array size by 1.
    // Switch only has to be done halfway-through since we don't need the removed
    // handle to exist anymore.
    for(size_t w = 0; w < application->windowCount ; ++w) {
        if(application->p_windows[w] == window) {
            if(w != application->windowCount - 1) {
                application->p_windows[w] = application->p_windows[application->windowCount - 1];
            }
            application->p_windows = bj_realloc(application->p_windows, --application->windowCount, application->p_allocator);
            break;
        }
    }
}
