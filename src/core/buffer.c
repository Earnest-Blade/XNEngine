#include "buffer.h"
#include "debug.h"

#include <stdio.h>
#include <memory.h>
#include <malloc.h>

#define XNE_GROWTH_FACTOR 2

void xne_create_vector(xne_Vector_t* vector, size_t elemsize, size_t capacity){
    xne_assert(vector);
    xne_assert(capacity >= 0 && elemsize > 0);

    memset(vector, 0, sizeof(xne_Vector_t));
    vector->count = 0;
    vector->capacity = capacity;
    vector->memory.elemsize = elemsize;
    vector->memory.size = capacity * elemsize * XNE_GROWTH_FACTOR;
    vector->memory.ptr = NULL;

    /* If the vector has a default capacity, a new memory location is allocated */
    if(capacity){
        vector->memory.ptr = malloc(vector->memory.size);
        xne_assert(vector->memory.ptr);

        memset(vector->memory.ptr, 0, vector->memory.size);
    }
}

void* xne_vector_push_back(xne_Vector_t* vector, void* element){
    xne_assert(vector);
    xne_assert(element);

    /* allocate new memory if the capacity is reached */
    if(vector->count >= vector->capacity){
        const size_t capacity = (vector->capacity + 1) * XNE_GROWTH_FACTOR;

        vector->memory.ptr = realloc(vector->memory.ptr, capacity * vector->memory.elemsize);
        xne_assert(vector->memory.ptr);
        
        vector->memory.size = capacity * vector->memory.elemsize;
        vector->capacity = capacity;
    }

    vector->count += 1;

    memcpy(&vector->memory.ptr[vector->count * vector->memory.elemsize], element, vector->memory.elemsize);
    return &vector->memory.ptr[vector->count * vector->memory.elemsize];
}

void* xne_vector_get(xne_Vector_t* vector, size_t index){
    xne_assert(vector->capacity > 0 && vector->capacity > index);
    return &vector->memory.ptr[index * vector->memory.elemsize];
}

void xne_destroy_vector(xne_Vector_t* vector){
    if(!vector) return;
    if(!vector->memory.ptr) return;

    /* try to free allocated memory */
    if(vector->memory.size){
        xne_vprintf("freeing a chunk of %i bits.", vector->memory.size);
        free(vector->memory.ptr);
    }
}

void xne_create_tree(xne_Tree_t* tree, void* value, size_t elemsize){
    xne_assert(tree);

    memset(tree, 0, sizeof(xne_Tree_t));
    tree->root = tree;
    tree->parent = NULL;
    tree->child_count = 0;
    tree->childs = NULL;
    tree->memory.elemsize = elemsize;
    tree->memory.size = elemsize;
    tree->memory.ptr = NULL;

    xne_tree_set_value(tree, value);
}

void xne_tree_fixed_childrens(xne_Tree_t* parent, size_t count){
    xne_assert(parent);

    parent->childs = malloc(sizeof(xne_Tree_t) * count);
    parent->child_count = count;

    xne_Tree_t* node;
    for (size_t i = 0; i < count; i++)
    {
        node = &parent->childs[i];
        node->root = parent->root;
        node->parent = parent;
        node->child_count = 0;
        node->childs = NULL;
        node->memory.elemsize = parent->memory.elemsize;
        node->memory.size = parent->memory.size;
        node->memory.ptr = NULL;

        xne_tree_set_value(node, NULL);
    }
}

xne_Tree_t* xne_tree_add_child(xne_Tree_t* parent, void* value){
    xne_assert(parent);

    if(parent->child_count > 0){
        parent->childs = realloc(parent->childs, sizeof(xne_Tree_t) * (parent->child_count + 1));
        xne_assert(parent->childs);
    }
    else {
        parent->childs = malloc(sizeof(xne_Tree_t));
        xne_assert(parent->childs);
    }

    parent->child_count++;

    xne_Tree_t* node = &parent->childs[parent->child_count];
    node->root = parent->root;
    node->parent = parent;
    node->child_count = 0;
    node->childs = NULL;
    node->memory.elemsize = parent->memory.elemsize;
    node->memory.size = parent->memory.size;
    node->memory.ptr = NULL;

    xne_tree_set_value(node, value);
    return node;
}

xne_Tree_t* xne_tree_get_child(xne_Tree_t* tree, size_t child){
    xne_assert(child >= 0 && child <= tree->child_count);
    return &tree->childs[child];
}

void xne_tree_set_value(xne_Tree_t* tree, void* value){
    if(!tree->memory.ptr){
        if(tree->memory.size){
            tree->memory.ptr = malloc(tree->memory.size);
            xne_assert(tree->memory.ptr);
            memset(tree->memory.ptr, 0, tree->memory.size);
        }
    }
    
    if(value){
        memcpy(tree->memory.ptr, value, tree->memory.size);
    }
}

void* xne_tree_get_value(xne_Tree_t* tree){
    xne_assert(tree);
    return tree->memory.ptr;
}

void xne_destroy_tree(xne_Tree_t* tree){
    if(tree){
        for (size_t i = 0; i < tree->child_count; i++)
        {
            xne_destroy_tree(&tree->childs[i]);
        }

        free(tree->childs);
        free(tree->memory.ptr);
    }
}