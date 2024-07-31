#pragma once

#ifndef XNE_SPRITE_IMPL
#define XNE_SPRITE_IMPL

#define XNE_CORE_FILE
#define XNE_CORE_COMPRESSION
#include "core/core.h"

#include "graphics/graphics.h"
#include "transform.h"

typedef enum xne_SpriteDirection {
    XNE_SPRITE_FORWARD = 0, 
    XNE_SPRITE_RIGHT = 1, 
    XNE_SPRITE_BACKWARD = 2,
    XNE_SPRITE_LEFT = 3
} xne_SpriteDirection_t;

typedef struct xne_SpriteAnimationTimelineMarker {
    uint32_t frame;
    float duration;
    double absolute_duration;
} xne_SpriteAnimationTimelineMarker_t;

typedef struct xne_SpriteAnimationTimeline {
    const char* name;
    xne_Vector_t markers;
    uint32_t current_marker, duration;
} xne_SpriteAnimationTimeline_t;

typedef struct xne_SpriteAnimationHandler {
    xne_Vector_t timelines;
    size_t current_animation;
    double current_frame;
} xne_SpriteAnimationHandler_t;

typedef struct xne_Sprite {
    xne_TextureAtlas_t atlas;
    xne_SpriteAnimationHandler_t animations;
    xne_Material_t material;
    xne_Mesh_t plane;
    xne_Transform_t transform;

    float width, height;
    uint32_t frame;
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

/*
    Create a new sprite structure from a descriptor.

    @param sprite Pointer to a previous allocated sprite structure.
    @param desc Sprite Descriptor.
    @return XNE_OK if the initialization succed, XNE_FAILURE otherwise.
*/
int xne_create_sprited(xne_Sprite_t* sprite, xne_SpriteDesc_t desc);

/*
    Create a new sprite structure from a file.

    @param sprite Pointer to a previous allocated sprite structure.
    @param file File stream.
    @return XNE_OK if the initialization succed, XNE_FAILURE otherwise.
*/
int xne_create_spritef(xne_Sprite_t* sprite, FILE* file);

/*
    Create a new sprite structure from a path.

    @param sprite Pointer to a previous allocated sprite structure.
    @param path relative or absolute path to the sprite file.
    @return XNE_OK if the initialization succed, XNE_FAILURE otherwise.
*/
static int xne_create_sprite(xne_Sprite_t* sprite, const char* path){
    FILE* file = fopen(path, "rb");
    int success = xne_create_spritef(sprite, file);
    fclose(file);
    return success;
}

/*
    Draw the sprite to the world space.
*/
void xne_draw_sprite(xne_Sprite_t* sprite);

/*
    Draw the sprite as a billboard, so the sprite will use a 'look at camera' matrix.
*/
void xne_draw_billboard_sprite(xne_Sprite_t* sprite, xne_Camera_t* camera);

/*
    Free the sprite from the memory.
*/
void xne_destroy_sprite(xne_Sprite_t* sprite);

/*
    Return true if the sprite has animations.
*/
static inline int xne_sprite_has_animations(xne_Sprite_t* sprite){
    return sprite->animations.timelines.count;
}

#endif