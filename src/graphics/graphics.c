#include "graphics.h"
#include "device.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef XNE_TARGET_OPENGL
    #include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#if _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_EXPOSE_NATIVE_WGL
    #include <Windows.h>
#else
    #define GLFW_NATIVE_INCLUDE_NONE
#endif
#include <GLFW/glfw3native.h>

#ifdef XNE_TARGET_OPENGL

static void GLAPIENTRY xne__gl_callback(GLenum source, GLenum type, uint32_t id, GLenum severity, int length, const char* message, const void* userParam){
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    /*char* src, sev;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             src = "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   src = "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: src = "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     src = "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     src = "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           src = "Source: Other"; break;
    };
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         sev = "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       sev = "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          sev = "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: sev = "Severity: notification"; break;
    };

    fprintf(stdout, "GL msg: (%i): %s\n%s\n%s\n", id, message, src, sev);*/
    fprintf(stdout, message);
}

#endif

void xne_create_graphic_device(xne_GraphicDevice_t* device, xne_GraphicDeviceDesc_t desc){
    xne_Device_t* window = (xne_Device_t*)desc.device;

#if _WIN32
    device->ctx = glfwGetWGLContext((GLFWwindow*) window->handle);
    device->device = GetDC(glfwGetWin32Window((GLFWwindow*) window->handle));

#ifdef XNE_TARGET_OPENGL

    /* if glfw does not create context */
    if(!device->ctx){
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32, // color depth
            0, 0, 0, 0, 0, 0, 
            0, 
            0, 
            0, 
            0, 0, 0, 0,
            24, // depthbuffer bits
            8, // stencil buffer bits
            0, // aux buffer count
            PFD_MAIN_PLANE,
            0, 
            0, 0, 0 
        };
        
        int pixelformat = ChoosePixelFormat((HDC)device->device, &pfd);
        SetPixelFormat((HDC)device->device, pixelformat, &pfd);
        device->ctx = wglCreateContext((HDC)device->device);
    }

    assert(device->ctx);
    assert(device->device);

    wglMakeCurrent((HDC)device->device, (HGLRC)device->ctx); 
    
    glfwSwapInterval(GLFW_TRUE);

    uint32_t status = glewInit();
    if(status != GLEW_OK){
        fprintf(stderr, "Failed to initiliaze GLEW: %s\n", glewGetErrorString(status));
    }

    fprintf(stdout, "Using GLEW '%s'\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "Using OpenGL '%s'\n", glGetString(GL_VERSION));

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(xne__gl_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

#endif

#else
    #error Unsupported OS
#endif

    device->draw;
    device->draw.clearf = XNE_CLEAR_COLOR;
    device->draw.framebuffer;
    memcpy(device->draw.clearc, desc.clear_color, sizeof(device->draw.clearc));
    xne_create_framebuffer(&device->draw.framebuffer, desc.framebuffer_shader, desc.buffer_width, desc.buffer_height);
}

void xne_new_frame(xne_GraphicDevice_t* device){
    xne_framebuffer_enable(&device->draw.framebuffer);
    xne_clear_framebuffer(&device->draw.framebuffer, device->draw.clearc[0], device->draw.clearc[1], device->draw.clearc[2]);
}

void xne_blit_buffers(xne_GraphicDevice_t* device){
    xne_framebuffer_disable(&device->draw.framebuffer);
    xne_blit_framebuffer(&device->draw.framebuffer);
}

void xne_destroy_graphic_device(xne_GraphicDevice_t* device){
    if(!device) return;
    // destroy framebuffer
    xne_destroy_framebuffer(&device->draw.framebuffer);
}

#ifdef XNE_TARGET_OPENGL

#if _WIN32

#endif

#endif