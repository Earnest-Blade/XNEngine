#pragma once

#ifndef XNE_GRAPHICS_IMPL
#define XNE_GRAPHICS_IMPL

#define XNE_TARGET_OPENGL

#include "core/core.h"

#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "framebuffer.h"
#include "camera.h"

typedef struct xne_GraphicDeviceDesc {
    void* device;
    float clear_color[3];
    uint32_t buffer_width, buffer_height;
    const char* framebuffer_shader;
} xne_GraphicDeviceDesc_t;

typedef enum xne_ClearFlags {
    XNE_CLEAR_COLOR = 0x00004000,
    XNE_CLEAR_DEPTH = 0x00000100
} xne_ClearFlags_t;

typedef struct xne_DrawContext {
    xne_Framebuffer_t framebuffer;
    xne_ClearFlags_t clearf;
    float clearc[3];
} xne_DrawContext_t;

typedef struct xne_GraphicDevice {
    void* ctx;
    void* device;
    xne_DrawContext_t draw;
} xne_GraphicDevice_t;

typedef struct xne_DrawCommand {
    uint32_t target;
    size_t elemCount;
    xne_Shader_t* shader;
    xne_Texture_t* texture;
} xne_DrawCommand_t;

void xne_create_graphic_device(xne_GraphicDevice_t* device, xne_GraphicDeviceDesc_t desc);
void xne_new_frame(xne_GraphicDevice_t* device);
void xne_blit_buffers(xne_GraphicDevice_t* device);
void xne_destroy_graphic_device(xne_GraphicDevice_t* device);

#endif