#pragma once

#ifndef XNE_BUFFER_IMPL
#define XNE_BUFFER_IMPL

#include "core/types.h"

#define XNE_DEFAULT_BUFFER_SIZE 1024

typedef enum xne_BufferTarget {
    XNE_BUFFER_TARGET_ARRAY = 0x8892, // GL_ARRAY_BUFFER
    XNE_BUFFER_TARGET_ATOMIC_COUNTER = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
    XNE_BUFFER_TARGET_COPY_READ = 0x8F36, // GL_COPY_READ
    XNE_BUFFER_TARGET_COPY_WRITE = 0x8F37, // GL_COPY_WRITE
    XNE_BUFFER_TARGET_ELEMENT_ARRAY = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
    XNE_BUFFER_TARGET_PIXEL_PACK = 0x88EB, // GL_PIXEL_PACK
    XNE_BUFFER_TARGET_PIXEL_UNPACK = 0x88EC, // GL_PIXEL_UNPACK
    XNE_BUFFER_TARGET_TEXTURE = 0x8C2A, // GL_TEXTURE_BUFFER
    XNE_BUFFER_TARGET_UNIFORM = 0x8A11  // GL_UNIFORM_BUFFER
} xne_BufferTarget_t;

typedef enum xne_BufferAccess {
    XNE_BUFFER_READ_ONLY = 0x88B8,
    XNE_BUFFER_WRITE_ONLY = 0x88B9,
    XNE_BUFFER_READ_WRITE = 0x88BA
} xne_BufferAccess_t;

typedef enum xne_BufferUsage {
    XNE_BUFFER_STREAM_DRAW = 0x88E0,
    XNE_BUFFER_STREAM_READ = 0x88E1,
    XNE_BUFFER_STREAM_COPY = 0x88E2,
    XNE_BUFFER_STATIC_DRAW = 0x88E4,
    XNE_BUFFER_STATIC_READ = 0x88E5,
    XNE_BUFFER_STATIC_COPY = 0x88E6,
    XNE_BUFFER_DYNAMIC_DRAW = 0x88E8,
    XNE_BUFFER_DYNAMIC_READ = 0x88E9,
    XNE_BUFFER_DYNAMIC_COPY = 0x88EA
} xne_BufferUsage_t;

struct xne_Buffer {
    char* ptr;
    size_t elemsize;
    size_t size;
};

/*
    Buffer use to save graphic data pointer.
*/
typedef struct xne_GraphicBuffer {
    struct xne_Buffer memory;
    uint32_t buffer;
    xne_Enum_t usage, acces, target;
} xne_GraphicBuffer_t;

/*
    Linear data structure that follows the 'Last In, First Out' principle.
*/
typedef struct xne_Vector {
    struct xne_Buffer memory;
    size_t count; // number of elements 
    size_t capacity; // max number of elements
} xne_Vector_t;

void xne_create_vector(xne_Vector_t* vector, size_t elemsize, size_t capacity);
static inline void xne_create_fixed_vector(xne_Vector_t* vector, size_t elemsize, size_t count){
    xne_create_vector(vector, elemsize, count);
    vector->count = count;
}

void* xne_vector_push_back(xne_Vector_t* vector, void* element);
void* xne_vector_get(xne_Vector_t* vector, size_t index);

inline void* xne_vector_front(xne_Vector_t* vector){
    return xne_vector_get(vector, 0);
}

inline void* xne_vector_back(xne_Vector_t* vector) {
    return xne_vector_get(vector, vector->count);
}

void xne_destroy_vector(xne_Vector_t* vector);

typedef struct xne_Tree {
    struct xne_Buffer memory;
    struct xne_Tree* childs;
    struct xne_Tree* root;
    struct xne_Tree* parent;
    size_t child_count;
} xne_Tree_t;

void xne_create_tree(xne_Tree_t* tree, void* value, size_t elemsize);
void xne_tree_fixed_childrens(xne_Tree_t* parent, size_t count);
xne_Tree_t* xne_tree_add_child(xne_Tree_t* parent, void* value);

void xne_tree_set_value(xne_Tree_t* tree, void* value);
void* xne_tree_get_value(xne_Tree_t* tree);

xne_Tree_t* xne_tree_get_child(xne_Tree_t* tree, size_t child);

static inline void* xne_tree_get_child_value(xne_Tree_t* tree, size_t child){
    return xne_tree_get_value(xne_tree_get_child(tree, child));
}

static inline int xne_tree_is_root(xne_Tree_t* tree){
    return tree == tree->root;
}

void xne_destroy_tree(xne_Tree_t* tree);

#endif