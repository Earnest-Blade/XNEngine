#pragma once

#ifndef XNE_OBJECTS_IMPL
#define XNE_OBJECTS_IMLP

#define XNE_CORE_BUFFER
#include "core/core.h"

#ifndef _json_h_
#include <json-c/json.h>
#endif

typedef json_object* xne_JsonPtr_t;

static xne_JsonPtr_t __json_context = NULL;

typedef enum xne_ObjectTypes {
    XNE_OBJECT_NULL = 0x0000,
    XNE_OBJECT_MODEL = 0x0001,
    XNE_OBJECT_SPRITE = 0x0002
} xne_ObjectTypes_t;

static void xne__object_create_vec2(const xne_JsonPtr_t __o, float* __dest){
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
}

static void xne__object_create_vec3(const xne_JsonPtr_t __o, float* __dest){
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
    __dest[2] = (float)json_object_get_double(json_object_object_get(__o, "Z"));
}

static void xne__object_create_vec4(const xne_JsonPtr_t __o, float* __dest){
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
    __dest[2] = (float)json_object_get_double(json_object_object_get(__o, "Z"));
    __dest[3] = (float)json_object_get_double(json_object_object_get(__o, "W"));
}

static void xne__object_create_quat(const xne_JsonPtr_t __o, float* __dest){
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[2] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
    __dest[3] = (float)json_object_get_double(json_object_object_get(__o, "Z"));
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "W"));
}

typedef struct xne_Node {
    const char* name;
    struct xne_Buffer buffer;
    xne_ObjectTypes_t type;
    xne_Transform_t transform;
} xne_Node_t;

typedef struct xne_Object {
    const char* name;
    xne_Tree_t root;
} xne_Object_t;

void xne_create_objectf(xne_Object_t* object, FILE* file);

void xne_destroy_object(xne_Object_t* object);

#endif