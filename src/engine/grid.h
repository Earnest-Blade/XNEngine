#pragma once

#ifndef XNE_GRID_IMPL
#define XNE_GRID_IMPL

#define XNE_INCLUDE_MESH_GEN
#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "transform.h"

typedef struct xne_Grid {
    xne_Mesh_t plane;
    xne_Shader_t shader;
    xne_Transform_t transform;

    uint32_t width, height, tile_width, tile_height;
    float color[3];
} xne_Grid_t;

int xne_create_grid(xne_Grid_t* grid, uint32_t width, uint32_t height, 
                    uint32_t tile_width, uint32_t tile_height);
void xne_draw_grid(xne_Grid_t* grid);
void xne_destroy_grid(xne_Grid_t* grid);

#endif