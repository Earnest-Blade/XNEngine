#include <stdarg.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>

#include "nk.h"
#include "ui.h"

#include "graphics/device.h"

void xne_create_user_device(xne_UserDevice_t* device, xne_UserDeviceDesc desc){
    const xne_Device_t* uid = (xne_Device_t*)desc.device;

    device->ctx = nk_glfw3_init((GLFWwindow*)uid->handle, NK_GLFW3_INSTALL_CALLBACKS);
    device->internal = NULL;
    device->atlas = (nk_font_atlas*) malloc(sizeof(nk_font_atlas));
    device->bufsize = desc.bufsize;
    device->AA = desc.AA;

    nk_glfw3_font_stash_begin(&device->atlas);

    nk_glfw3_font_stash_end();
}

void xne_user_device_new_frame(xne_UserDevice_t* device){
    nk_glfw3_new_frame();
}

void xne_user_device_draw(xne_UserDevice_t* device){
    nk_glfw3_render((nk_anti_aliasing)device->AA, device->bufsize, device->bufsize);
}

void xne_destroy_user_device(xne_UserDevice_t* device){
    if(!device) return;
    nk_glfw3_shutdown();
    free(device->atlas);
}