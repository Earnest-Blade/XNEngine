#pragma once

#ifndef XNE_UIX_IMPL
#define XNE_UIX_IMPL

#include "core/math.h"
#include "font.h"

#define XNE_COLOR_WHITE (float[3]){1.0f, 1.0f, 1.0f}

typedef struct xne_UserInterface {
    struct {
        uint32_t dev, frag, vert;
        uint8_t vertex_attrib;
        uint8_t matrix_uniform, color_uniform, texture_uniform;
    } shader;

    struct {
        uint32_t x, y, width, height;
    } scissors;
    
    struct xne_Buffer memory;

    uint32_t vertex_object, buffer_object;

    // 0 -> unpack buffer
    // 1 -> pack buffer 
    uint32_t pixel_object[2];
    
    uint32_t texture_object;
    
    xne_mat4 projection;
} xne_UserInterface_t;
 
int xne_create_uix_instance(size_t default_buffer_size);
void xne_uix_new_frame(uint32_t width, uint32_t height);

void xne_draw_font_atlas(xne_Font_t* font);

void xne_draw_text_scaled(xne_Font_t* font, const char* text, float x, float y, float scale, xne_Colorf_t color);
static inline void xne_draw_text(xne_Font_t* font, const char* text, float x, float y, xne_Colorf_t color){
    xne_draw_text_scaled(font, text, x, y, 1.0f, color);
}

void xne_destroy_uix_instance();

#endif