#pragma once

#ifndef XNE_OBJECTS_IMPL
#define XNE_OBJECTS_IMLP

#define XNE_CORE_BUFFER
#include "core/core.h"

#ifndef _json_h_
#include <json-c/json.h>
#endif

static json_object* __json_context = NULL;

typedef enum xne_ObjectTypes {
    XNE_OBJECT_NULL = 0x0000,
    XNE_OBJECT_MODEL = 0x0001,
    XNE_OBJECT_SPRITE = 0x0002,
    XNE_OBJECT_SCENE = 0x0004
} xne_ObjectTypes_t;

/*
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
*/

static void xne__object_create_vec2(const json_object* __o, float* __dest){
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
}

static void xne__object_create_vec3(const json_object* __o, float* __dest){
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
    __dest[2] = (float)json_object_get_double(json_object_object_get(__o, "Z"));
}

static void xne__object_create_vec4(const json_object* __o, float* __dest){
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
    __dest[2] = (float)json_object_get_double(json_object_object_get(__o, "Z"));
    __dest[3] = (float)json_object_get_double(json_object_object_get(__o, "W"));
}

static void xne__object_create_quat(const json_object* __o, float* __dest){
    __dest[1] = (float)json_object_get_double(json_object_object_get(__o, "X"));
    __dest[2] = (float)json_object_get_double(json_object_object_get(__o, "Y"));
    __dest[3] = (float)json_object_get_double(json_object_object_get(__o, "Z"));
    __dest[0] = (float)json_object_get_double(json_object_object_get(__o, "W"));
}

/*
    Check if an 'xneo' object's type correspond to another type.
    @param root Json pointer to the root of the json file.
    @param type Type to compare with.
    @return Return XNE_OK if the object is the correct type, XNE_FAILURE otherwise.
*/
static inline int xne__object_is_type_of(json_object* root, xne_ObjectTypes_t type){
    json_object* scene = json_object_object_get(root, "Asset");
    if(json_object_get_type(scene) == json_type_null) {
        return XNE_FAILURE;
    }

    return !json_object_get_int(json_object_object_get(scene, "Type")) == type;
}

int xne__create_transform_from_object(const json_object* root, void* dest);
int xne__create_material_from_object(const json_object* root, void* dest);

#endif