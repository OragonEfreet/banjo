#include <_rfu/app.h>
#include <_rfu/window.h>
#include <errors.h>
#include <memory.h>

#include <banjo/api.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>



void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

BjResult bjCreateWindow(
    const BjWindowInfo* pInfo,
    BjWindow*                 pWindow
) {
    bjExpectValue(pInfo,              BJ_NULL_CREATE_INFO);
    bjExpectValue(pInfo->application, BJ_INVALID_CREATE_INFO);
    bjExpectValue(pInfo->width,       BJ_INVALID_WINDOW_SIZE);
    bjExpectValue(pInfo->height,      BJ_INVALID_WINDOW_SIZE);


    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* handle = glfwCreateWindow(
        pInfo->width, pInfo->height,
        pInfo->title ? pInfo->title : pInfo->application->name,
        NULL, NULL
    );

    struct BjWindow_T* window = bjNewStruct(BjWindow, pInfo->application->pAllocator);
    window->handle = handle;
    window->application = pInfo->application;

    bj_LinkWindow(pInfo->application, window);

    *pWindow = window;
    return BJ_SUCCESS;
}

BjResult bjDestroyWindow(
    BjWindow window
) {
    bjExpectValue(window, BJ_NULL_INPUT_HANDLE);

    bj_UnlinkWindow(window->application, window);
    glfwDestroyWindow(window->handle);

    bjFree(window, window->application->pAllocator);
    return BJ_SUCCESS;
}

bool bjWindowShouldClose(
    BjWindow window
) {
    return glfwWindowShouldClose(window->handle);
}

void bj_ProcessEvents() {
    glfwPollEvents();
}

static usize n_instances = 0;

BjResult bj_RetainWindowComponent(BjApplication application)
{
    if(n_instances++ == 0) {
        glfwSetErrorCallback(error_callback);
        if(!glfwInit()) {
            return BJ_PLATFORM_ERROR;
        }
    }
    return BJ_SUCCESS;
}

BjResult bj_ReleaseWindowComponent(BjApplication application)
{
    if(--n_instances == 0) {
        glfwTerminate();
    }
}

