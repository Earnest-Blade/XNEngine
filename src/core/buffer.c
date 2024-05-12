#include "buffer.h"

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <assert.h>

#define XNE_GROWTH_FACTOR 2

void xne_create_vector(xne_Vector_t* vector, size_t elemsize, size_t capacity){
    assert(vector);
    assert(capacity >= 0 && elemsize > 0);

    memset(vector, 0, sizeof(xne_Vector_t));
    vector->count = 0;
    vector->capacity = capacity;
    vector->memory.elemsize = elemsize;
    vector->memory.size = capacity * elemsize * XNE_GROWTH_FACTOR;
    vector->memory.ptr = NULL;

    /* If the vector has a default capacity, a new memory location is allocated */
    if(capacity){
        vector->memory.ptr = malloc(vector->memory.size);
        assert(vector->memory.ptr);

        memset(vector->memory.ptr, 0, vector->memory.size);
    }
}

void* xne_vector_push_back(xne_Vector_t* vector, void* element){
    assert(vector);
    assert(element);

    /* allocate new memory if the capacity is reached */
    if(vector->count >= vector->capacity){
        const size_t capacity = (vector->capacity + 1) * XNE_GROWTH_FACTOR;

        vector->memory.ptr = realloc(vector->memory.ptr, capacity * vector->memory.elemsize);
        assert(vector->memory.ptr);
        
        vector->memory.size = capacity * vector->memory.elemsize;
        vector->capacity = capacity;
    }

    vector->count += 1;

    memcpy(&vector->memory.ptr[vector->count * vector->memory.elemsize], element, vector->memory.elemsize);
    return &vector->memory.ptr[vector->count * vector->memory.elemsize];
}

void* xne_vector_get(xne_Vector_t* vector, size_t index){
    assert(vector->capacity > 0 && vector->capacity > index);
    return &vector->memory.ptr[index * vector->memory.elemsize];
}

void xne_destroy_vector(xne_Vector_t* vector){
    if(!vector) return;

    /* try to free allocated memory */
    if(vector->memory.size){
        free(vector->memory.ptr);
    }
}