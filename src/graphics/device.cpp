#include "device.h"

#include <assert.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>

#include <GLFW/glfw3.h>

#define XNE_KEYBOARD_SIZE 512
#define XNE_MOUSE_SIZE 16
#define XNE_KEY_NO_STATE 0

static struct xne__DeviceInternal {
    xne_Device* window;
    void* handle;
    struct xne__input_device {
        char keyStates[XNE_KEYBOARD_SIZE];
        char mouseStates[XNE_MOUSE_SIZE];
        float scroll, sensivity;
        double lastMousePosition[2];
        double mousePosition[2];
    } device;
    char done;
    char focused;
} __glfw;

static void xne__key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    __glfw.device.keyStates[key] = action;
}

static void xne__mouse_callback(GLFWwindow* window, int button, int action, int mods){
    __glfw.device.mouseStates[button] = action;
}

static void xne__cursor_pos_callback(GLFWwindow* window, double x, double y){
    __glfw.device.mousePosition[0] = x;
    __glfw.device.mousePosition[1] = y;
}

static void xne__scroll_callback(GLFWwindow* window, double xoff, double yoff){
    __glfw.device.scroll = (float)yoff;
}

static void xne__focus_callback(GLFWwindow* window, int focused){
    __glfw.focused = focused;
}

static void xne__client_size_callback(GLFWwindow* window, int width, int height){
    __glfw.window->clientSize[0] = width;
    __glfw.window->clientSize[1] = height;
}

static void xne__client_pos_callback(GLFWwindow* window, int x, int y){
    __glfw.window->clientPosition[0] = x;
    __glfw.window->clientPosition[1] = y;
}

static void xne__framebuffer_callback(GLFWwindow* window, int width, int height){
    __glfw.window->framebufferSize[0] = width;
    __glfw.window->framebufferSize[1] = height;
}

void xne_create_device(xne_Device_t* device, const int width, const int height, const char* title, uint32_t flags){
    assert(device);
    assert(glfwInit());
    assert(width > 0); 
    assert(height > 0);

#ifdef XNE_TARGET_OPENGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    /*

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    
    */
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    if(flags == XNE_WINDOW_NONE) glfwDefaultWindowHints();
    else {
        glfwWindowHint(GLFW_RESIZABLE, (flags & (int)XNE_WINDOW_RESIZABLE) == XNE_WINDOW_RESIZABLE);
        glfwWindowHint(GLFW_VISIBLE, (flags & (int)XNE_WINDOW_VISIBLE) == XNE_WINDOW_VISIBLE);
        glfwWindowHint(GLFW_DECORATED, (flags & (int)XNE_WINDOW_DECORATED) == XNE_WINDOW_DECORATED);
        glfwWindowHint(GLFW_FOCUSED, (flags & (int)XNE_WINDOW_FOCUSED) == XNE_WINDOW_FOCUSED);
        glfwWindowHint(GLFW_AUTO_ICONIFY, (flags & (int)XNE_WINDOW_AUTO_ICONIFY) == XNE_WINDOW_AUTO_ICONIFY);
        glfwWindowHint(GLFW_FLOATING, (flags & (int)XNE_WINDOW_FLOATING) == XNE_WINDOW_FLOATING);
        glfwWindowHint(GLFW_CENTER_CURSOR, (flags & (int)XNE_WINDOW_CENTER_CUSOR) == XNE_WINDOW_CENTER_CUSOR);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, (flags & (int)XNE_WINDOW_FOCUS_ON_SHOW) == XNE_WINDOW_FOCUS_ON_SHOW);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, (flags & (int)XNE_WINDOW_SCALE_TO_MONITOR) == XNE_WINDOW_SCALE_TO_MONITOR);
    }
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);  
#ifdef _WIN32
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif

    device->handle = glfwCreateWindow((int)width, (int)height, title, NULL, NULL);
    assert(device->handle);

    device->title = (char*) malloc(strlen(title) + 1);
    strcpy(device->title, title);

    device->clientSize[0] = (int)width;
    device->clientSize[1] = (int)height;
    glfwGetWindowPos((GLFWwindow*)device->handle, &device->clientPosition[0], &device->clientPosition[1]);
    glfwGetFramebufferSize((GLFWwindow*)device->handle, &device->framebufferSize[0], &device->framebufferSize[1]);
    glfwSetWindowUserPointer((GLFWwindow*)device->handle, device);

    if((flags & (int)XNE_WINDOW_FULLSCREEN) == XNE_WINDOW_FULLSCREEN){
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor((GLFWwindow*)device->handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    glfwSetWindowFocusCallback((GLFWwindow*)device->handle, xne__focus_callback);
    glfwSetWindowSizeCallback((GLFWwindow*)device->handle, xne__client_size_callback);
    glfwSetWindowPosCallback((GLFWwindow*)device->handle, xne__client_pos_callback);
    glfwSetFramebufferSizeCallback((GLFWwindow*)device->handle, xne__framebuffer_callback);

    glfwSetKeyCallback((GLFWwindow*)device->handle, xne__key_callback);
    glfwSetMouseButtonCallback((GLFWwindow*)device->handle, xne__mouse_callback);
    glfwSetCursorPosCallback((GLFWwindow*)device->handle, xne__cursor_pos_callback);
    glfwSetScrollCallback((GLFWwindow*)device->handle, xne__scroll_callback);

    __glfw.window = device;
    __glfw.handle = device->handle;
    __glfw.done = false;
    __glfw.focused = true;
    __glfw.device.scroll = 0.0f;
    __glfw.device.sensivity = 1.0f;
    memset(__glfw.device.keyStates, XNE_KEY_NO_STATE, XNE_KEYBOARD_SIZE);
    memset(__glfw.device.mouseStates, XNE_KEY_NO_STATE, XNE_MOUSE_SIZE);
    memset(__glfw.device.lastMousePosition, 0, sizeof(double) * 2);
    memset(__glfw.device.mousePosition, 0, sizeof(double) * 2);
    glfwGetCursorPos((GLFWwindow*)device->handle, &__glfw.device.mousePosition[0], &__glfw.device.mousePosition[1]);
}

void xne_poll_events(xne_Device_t* device){
    glfwPollEvents();
}

void xne_swap_buffers(xne_Device_t* device){
    glfwSwapBuffers((GLFWwindow*)device->handle);
    memcpy(__glfw.device.lastMousePosition, __glfw.device.mousePosition, sizeof(double) * 2);
    __glfw.device.scroll = 0.0f;
}

void xne_destroy_device(xne_Device_t* device){
    if(!device) return;
    free(device->title);
    glfwDestroyWindow((GLFWwindow*)device->handle);
    glfwTerminate();
}

void xne_set_device_title(xne_Device_t* device, const char* title) {
    free(device->title);
    device->title = (char*) malloc(strlen(title) + 1);
    strcpy(device->title, title);
    device->title[strlen(title)] = '\0';

    glfwSetWindowTitle((GLFWwindow*)device->handle, title);
}

void xne_set_device_client_size(xne_Device_t* device, const int width, const int height){
    assert(width > 0);
    assert(height > 0);
    
    device->clientSize[0] = width;
    device->clientSize[1] = height;
    glfwSetWindowSize((GLFWwindow*)device->handle, width, height);
}

void xne_set_device_client_position(xne_Device_t* device, const int x, const int y){
    assert(x > 0);
    assert(y > 0);

    device->clientPosition[0] = x;
    device->clientPosition[1] = y;
    glfwSetWindowPos((GLFWwindow*)device->handle, x, y);
}

bool xne_is_done(void) { return __glfw.done || glfwWindowShouldClose((GLFWwindow*)__glfw.handle); }
void xne_done(void) { __glfw.done = true; }
bool xne_is_focus(void) { return __glfw.focused; }
double xne_get_time(void) { return glfwGetTime(); }

char xne_key_down(xne_Keys_t key) { return (char) __glfw.device.keyStates[key]; }
char xne_key_pressed(xne_Keys_t key) { return (char) __glfw.device.keyStates[key] == GLFW_PRESS; }
char xne_key_released(xne_Keys_t key) { return (char) __glfw.device.keyStates[key] == GLFW_RELEASE; }

char xne_mouse_button_down(xne_MouseButtons_t button) { return __glfw.device.mouseStates[button]; }
char xne_mouse_button_pressed(xne_MouseButtons_t button) { return __glfw.device.mouseStates[button] == GLFW_PRESS; }
char xne_mouse_button_released(xne_MouseButtons_t button) { return __glfw.device.mouseStates[button] == GLFW_RELEASE; }

void xne_get_mouse_position(int* x, int* y){
    *x = (int)__glfw.device.mousePosition[0] * __glfw.device.sensivity;
    *y = (int)__glfw.device.mousePosition[1] * __glfw.device.sensivity;
}

void xne_get_mouse_delta_position(float* x, float* y){
    *x = (float)(__glfw.device.mousePosition[0] - __glfw.device.lastMousePosition[0]) * __glfw.device.sensivity;
    *y = (float)(__glfw.device.mousePosition[1] - __glfw.device.lastMousePosition[1]) * __glfw.device.sensivity;
}

int xne_get_mouse_scroll(){
    return __glfw.device.scroll * __glfw.device.sensivity;
}