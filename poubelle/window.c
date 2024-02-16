#include <_rfu/app.h>
#include <_rfu/window.h>
#include <banjo/error.h>
#include <memory.h>

#include <banjo/api.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>



void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

BjResult bj_window_create(
    const BjWindowInfo* p_info,
    BjWindow*                 pWindow
) {
    bjAssert(p_info != 0);


    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* handle = glfwCreateWindow(
        p_info->width, p_info->height,
        p_info->title ? p_info->title : p_info->application->name,
        NULL, NULL
    );

    struct BjWindow_T* window = bj_new_struct(BjWindow, p_info->application->p_allocator);
    window->handle = handle;
    window->application = p_info->application;

    bj_window_link(p_info->application, window);

    *pWindow = window;
    return BJ_SUCCESS;
}

BjResult bj_window_destroy(
    BjWindow window
) {
    bjAssert(window != 0);

    bj_window_unlink(window->application, window);
    glfwDestroyWindow(window->handle);

    bj_free(window, window->application->p_allocator);
    return BJ_SUCCESS;
}

bool bj_window_should_close(
    BjWindow window
) {
    return glfwWindowShouldClose(window->handle);
}

void bj_ProcessEvents() {
    glfwPollEvents();
}

static usize n_instances = 0;

BjResult bj_window_retain(BjApplication application)
{
    if(n_instances++ == 0) {
        glfwSetErrorCallback(error_callback);
        if(!glfwInit()) {
            return BJ_PLATFORM_ERROR;
        }
    }
    return BJ_SUCCESS;
}

BjResult bj_window_release(BjApplication application)
{
    if(--n_instances == 0) {
        glfwTerminate();
    }
}

