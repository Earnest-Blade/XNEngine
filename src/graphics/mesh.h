#pragma once

#ifndef XNE_MESH_IMPL
#define XNE_MESH_IMPL

#define XNE_CORE_BUFFER
#define XNE_CORE_FILE
#include "core/core.h"
#include "core/buffer.h"

#include "texture.h"
#include "shader.h"

enum xne_MeshPrimitive  {
    XNE_MESH_PRIMITIVE_POINT = 0,
    XNE_MESH_PRIMITIVE_LINE = 1,
    XNE_MESH_PRIMITIVE_TRIANGLE = 4,
    XNE_MESH_PRIMITIVE_QUAD = 7
};

enum xne_VertexLayoutAttrib  {
    XNE_VERTEX_POSITION,
    XNE_VERTEX_TEXCOORD,
    XNE_VERTEX_NORMAL,
    XNE_VERTEX_COLOR,
    XNE_VERTEX_ATTRIB_END
};

typedef struct xne_Vertex {
    float position[3];
    float uv[2];
    float normal[3];
} xne_Vertex_t;

#define XNE_VERTEX_ALIGN_END(_offset) XNE_VERTEX_ATTRIB_END, XNE_FLOAT, _offset
typedef struct xne_VertexAlignDesc {
    xne_Enum_t attrib;
    xne_Enum_t format;
    size_t offset;
} xne_VertexAlignDesc_t;

typedef struct xne_MeshDesc {
    const void* vertices;
    const uint32_t* elements;
    const xne_VertexAlignDesc_t* vertex_align;
    size_t vertices_count, elements_count;
    xne_Enum_t primitive;
} xne_MeshDesc_t;

typedef struct xne_Mesh {
    uint32_t target, attrib_count;
    xne_GraphicBuffer_t vertices, elements;
    xne_Enum_t primitive;
} xne_Mesh_t;

void xne_create_mesh(xne_Mesh_t* mesh, xne_MeshDesc_t desc);
void xne_draw_mesh(xne_Mesh_t* mesh);
void xne_destroy_mesh(xne_Mesh_t* mesh);

/*typedef struct xne_Model {
    xne_Mesh_t** meshes;
    xne_Texture_t** textures;
    xne_Material_t** materials;
    
    struct xne__ModelNode {
        struct xne__ModelNode** childs;
        uint32_t* meshes;
        uint32_t material;
        char* name;
        size_t cntChild, cntMesh;
    } root;

    size_t cntMesh, cntTextures, cntMaterials;
} xne_Model_t;

void xne_create_modelf(xne_Model_t* model, FILE* file);

inline void xne_create_model(xne_Model_t* model, const char* path){
    FILE* file = fopen(path, "rb");
    xne_create_modelf(model, file);
    fclose(file);
}

void xne_draw_model(xne_Model_t* model);
void xne_destroy_model(xne_Model_t* model);*/

#ifdef XNE_INLUDE_MESH_GEN

inline static void xne_create_plane(xne_Mesh_t* dest, const float width, const float height){
    const xne_VertexAlignDesc_t align[] = {
        { XNE_VERTEX_POSITION, XNE_FLOAT, offsetof(xne_Vertex_t, position) },
        { XNE_VERTEX_TEXCOORD, XNE_FLOAT, offsetof(xne_Vertex_t, uv) },
        { XNE_VERTEX_NORMAL, XNE_FLOAT, offsetof(xne_Vertex_t, normal) },
        { XNE_VERTEX_ALIGN_END(sizeof(xne_Vertex_t)) }
    };

    const float vertices[32] = {
        -width,  height, 0, 0, 1, 0, 0, 1,
        -width, -height, 0, 0, 0, 0, 0, 1,
         width, -height, 0, 1, 0, 0, 0, 1,
         width,  height, 0, 1, 1, 0, 0, 1
    };
    const uint32_t elems[6] = { 0, 1, 2, 0, 2, 3 };

    xne_MeshDesc_t mdesc;
    mdesc.vertices = vertices;
    mdesc.elements = elems;
    mdesc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;
    mdesc.vertex_align = align;
    mdesc.vertices_count = 4;
    mdesc.elements_count = 6;

    xne_create_mesh(dest, mdesc);
}

#endif

#endif