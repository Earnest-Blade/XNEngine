#pragma once

#ifndef XNE_MAT_IMPL
#define XNE_MAT_IMPL

#include "graphics/shader.h"

typedef struct xne_Material {
    xne_Shader_t shader;
    struct {
        uint16_t projection;
        uint16_t transform;
        uint16_t world;
        uint16_t directional_light;
    } uniforms;
    
    struct xne_Texture* diffuse_texture;
    struct xne_Texture* ambient_texture;

    float diffuse_color[4];
    float ambient_color[4];
} xne_Material_t;

static inline void xne_destroy_material(xne_Material_t* material){
    xne_destroy_shader(&material->shader);
}



#endif