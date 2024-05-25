#pragma once
#ifndef XNE_FRAMEBUFFER_IMPL
#define XNE_FRAMEBUFFER_IMPL

#include "core/core.h"

#include "shader.h"

typedef struct xne_Framebuffer {
    uint32_t width, height, prev_width, prev_height;
    uint32_t buffer, color_buffer, depth_buffer;
    uint32_t vertex_object, array_object;
    xne_Shader_t shader;
} xne_Framebuffer_t;

void xne_create_framebuffer(xne_Framebuffer_t* framebuffer, const char* shader, uint32_t width, uint32_t height);
void xne_framebuffer_enable(xne_Framebuffer_t* framebuffer);
void xne_framebuffer_disable(xne_Framebuffer_t* framebuffer);
void xne_clear_framebuffer(xne_Framebuffer_t* framebuffer, xne_Color_t color);
void xne_blit_framebuffer(xne_Framebuffer_t* framebuffer);
void xne_destroy_framebuffer(xne_Framebuffer_t* framebuffer);

#endif