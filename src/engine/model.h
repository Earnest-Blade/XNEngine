#pragma once

#ifndef XNE_MODEL_IMPL
#define XNE_MODEL_IMPL

#define XNE_CORE_BUFFER
#define XNE_CORE_FILE
#define XNE_CORE_COMPRESSION
#include "core/core.h"

#include "transform.h"

#include <stdio.h>

typedef struct xne_Model {
    struct xne__Model_Node {
        char* name;

        xne_Vector_t childs;
        xne_Transform_t transform;

        struct xne_Mesh* mesh;
        struct xne_Material* material;
    } root;

    xne_Vector_t meshes;
    xne_Vector_t textures;
    xne_Vector_t materials;
} xne_Model_t;

int xne_create_modelf(xne_Model_t* model, FILE* file);
static int xne_create_model(xne_Model_t* model, const char* path){
    FILE* file = fopen(path, "rb");
    int success = xne_create_modelf(model, file);
    fclose(file);

    return success;
}

void xne_draw_model(xne_Model_t* model);
void xne_destroy_model(xne_Model_t* model);

static inline struct xne__Model_Node* xne_get_node_childs(struct xne__Model_Node* node, uint32_t index){
    return (struct xne__Model_Node*)xne_vector_get(&node->childs, index);
}

#endif