#pragma once

#ifndef XNE_SPRITE_IMPL
#define XNE_SPRITE_IMPL

#define XNE_CORE_FILE
#define XNE_CORE_COMPRESSION
#include "core/core.h"

#include "graphics/graphics.h"
#include "transform.h"

typedef struct xne_Sprite {
    xne_TextureAtlas_t textures;
    xne_Shader_t shader;
    xne_Mesh_t plane;
    xne_Transform_t transform;

    float width, height;
} xne_Sprite_t;

typedef struct xne_SpriteDesc {
    const char* path;
    const char* shader;
    float sprite_width, sprite_height;
    uint16_t tile_width_count, tile_height_count;
    
    xne_ShaderDesc_t* shader_desc;
    xne_ShaderUniformDesc_t* uniform_desc;
    xne_TextureWrap_t wrap;
    xne_TextureFilter_t filter;
} xne_SpriteDesc_t;

int xne_create_sprited(xne_Sprite_t* sprite, xne_SpriteDesc_t desc);
int xne_create_spritef(xne_Sprite_t* sprite, FILE* file);
static int xne_create_sprite(xne_Sprite_t* sprite, const char* path){
    FILE* file = fopen(path, "rb");
    int success = xne_create_spritef(sprite, file);
    fclose(file);
    return success;
}

void xne_draw_sprite(xne_Sprite_t* sprite, xne_Camera_t* camera);
void xne_destroy_sprite(xne_Sprite_t* sprite);

#endif