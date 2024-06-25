#include "mesh.h"

#define XNE_CORE_FILE
#define XNE_CORE_STRING
#include "core/core.h"

#include <zlib.h>
#include <json-c/json.h>

#include <assert.h>
#include <string.h>
#include <GL/glew.h>

#define XNE_BUFFER_SIZE 255
#define XNE_DEFAULT_ALIGN_SIZE 32

#pragma region mesh

void xne_create_mesh(xne_Mesh_t* mesh, xne_MeshDesc_t desc) {
    assert(mesh);
    assert(desc.vertices_count > 0);
    assert(desc.elements_count > 0);
    assert(desc.vertices);
    assert(desc.elements);
    mesh->vertices;
    mesh->elements;
    mesh->attrib_count = 0;
    mesh->target = 0;
    mesh->primitive = desc.primitive;

    glGenVertexArrays(1, &mesh->target);
    glBindVertexArray(mesh->target);

    glGenBuffers(1, &mesh->vertices.buffer);
    mesh->vertices.target = XNE_BUFFER_TARGET_ARRAY;
    mesh->vertices.acces = XNE_BUFFER_READ_WRITE;
    mesh->vertices.usage = XNE_BUFFER_STATIC_DRAW;
    
    /* find attrib count */
    xne_VertexAlignDesc_t* align = (xne_VertexAlignDesc_t*) &desc.vertex_align[0];
    while (align->attrib != XNE_VERTEX_ATTRIB_END){        
        align = (xne_VertexAlignDesc_t*) &desc.vertex_align[++mesh->attrib_count];
    }

    mesh->vertices.memory.ptr = NULL;
    mesh->vertices.memory.elemsize = desc.vertex_align[mesh->attrib_count].offset;
    mesh->vertices.memory.size = mesh->vertices.memory.elemsize * desc.vertices_count;

    glBindBuffer(mesh->vertices.target, mesh->vertices.buffer);
    glBufferData(mesh->vertices.target, mesh->vertices.memory.size, desc.vertices, mesh->vertices.usage);

    /* attrib */
    for (size_t i = 0; i < mesh->attrib_count; i++)
    {
        glEnableVertexAttribArray(i);

        int size = 
            desc.vertex_align[i + 1].offset / XNE_GET_SIZE((xne_Types_t)desc.vertex_align[i + 1].format) -
            desc.vertex_align[i].offset / XNE_GET_SIZE((xne_Types_t)desc.vertex_align[i].format);

        glVertexAttribPointer(i, size, desc.vertex_align[i].format, GL_FALSE, mesh->vertices.memory.elemsize, (void*)desc.vertex_align[i].offset);
    }

    /* create element buffer */
    glGenBuffers(1, &mesh->elements.buffer);
    mesh->elements.target = XNE_BUFFER_TARGET_ELEMENT_ARRAY;
    mesh->elements.acces = XNE_BUFFER_READ_WRITE;
    mesh->elements.usage = XNE_BUFFER_STATIC_DRAW;
    mesh->elements.memory.ptr = NULL;
    mesh->elements.memory.elemsize = sizeof(uint32_t);
    mesh->elements.memory.size = sizeof(uint32_t) * desc.elements_count;

    glBindBuffer(mesh->elements.target, mesh->elements.buffer);
    glBufferData(mesh->elements.target, sizeof(uint32_t) * desc.elements_count, desc.elements, mesh->elements.usage);

    for (size_t i = 0; i < mesh->attrib_count; i++)
    {
        glDisableVertexAttribArray(i);
    }

    glBindBuffer(mesh->elements.target, 0);
    glBindBuffer(mesh->elements.target, 0);
    glBindVertexArray(0);    
}

void xne_draw_mesh(xne_Mesh_t* mesh){
    glBindVertexArray(mesh->target);
    glBindBuffer(mesh->vertices.target, mesh->vertices.buffer);
    glBindBuffer(mesh->elements.target, mesh->elements.buffer);

    for (size_t i = 0; i < mesh->attrib_count; i++)
        glEnableVertexAttribArray(i);
    
    glDrawElements(mesh->primitive, mesh->elements.memory.size / mesh->elements.memory.elemsize, XNE_UNSIGNED_INT, NULL);

    for (size_t i = 0; i < mesh->attrib_count; i++)
        glDisableVertexAttribArray(i);
    
    glBindBuffer(mesh->elements.target, 0);
    glBindBuffer(mesh->vertices.target, 0);
    glBindVertexArray(0);
}

void xne_destroy_mesh(xne_Mesh_t* mesh){
    glDeleteBuffers(1, &mesh->elements.buffer);
    glDeleteBuffers(1, &mesh->vertices.buffer);
    glDeleteVertexArrays(1, &mesh->target);
}

#pragma endregion