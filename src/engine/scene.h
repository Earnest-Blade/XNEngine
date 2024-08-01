#pragma once

#ifndef XNE_SCENE_IMPL
#define XNE_SCENE_IMPL

#define XNE_CORE_BUFFER
#define XNE_CORE_FILE
#define XNE_CORE_COMPRESSION
#include "core/core.h"

#include "graphics/camera.h"
#include "graphics/lights.h"

typedef enum xne_SceneFunctionType {
    XNE_INIT_FUNC,
    XNE_UPDATE_FUNC,
    XNE_DRAW_FUNC,
    XNE_DESTROY_FUNC
} xne_SceneFunctionType_t;

typedef struct xne_Scene {
    char* name;
    struct xne_Buffer memory;
    xne_Camera_t camera;

    xne_Light_t directional_light;

    void (*initialization_func)(struct xne_Scene*);
    void (*update_func)(struct xne_Scene*, float);
    void (*draw_func)(struct xne_Scene*);
    void (*destroy_func)(struct xne_Scene*);
} xne_Scene_t;

/*
    Create manually a new scene into a previous allocated struct.
    @param scene Pointer to the scene structure.
    @param name Scene's name.
    @param camera Camera descriptor structure use to create the default scene's camera.
    @return XNE_OK if the scene is successfully created, XNE_FAILTURE otherwise.
*/
int xne_create_scened(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera);

/*
    Create a new scene from a file, into a previous allocated struct.
    @param scene Pointer to the scene structure.
    @param file File's pointer.
    @return XNE_OK if the scene is successfully created, XNE_FAILTURE otherwise.
*/
int xne_create_scenef(xne_Scene_t* scene, FILE* file);

/*
    Create a new scene with a path, into a previous allocated struct.
    @param scene Pointer to the scene structure.
    @param path Path to read the scene from.
    @return XNE_OK if the scene is successfully created, XNE_FAILTURE otherwise.
*/
static inline int xne_create_scene(xne_Scene_t* scene, const char* path){
    FILE* f = fopen(path, "rb");
    int success = xne_create_scenef(scene, f);
    fclose(f);
    return success;
}

/*
    Allocate a new memory chunck use to store scene's data. 
    You can use this memory to store scene's relatives structures such as models or sprites.
    @param scene Pointer to the scene structure.
    @param size Needed chunck's size.
    @param value Default chunck's value. Can be NULL.
    @return Pointer to the new memory buffer if successfully allocated, NULL pointer otherwise.
*/
void* xne_scene_allocate_buffer(xne_Scene_t* scene, size_t size, void* value);

/*
    Return a pointer to the scene's memory buffer.
    @param scene Pointer to the scene structure.
    @return Pointer to the scene's memory buffer if it exists, a NULL pointer otherwise.
*/
void* xne_scene_get_buffer(xne_Scene_t* scene);

/*
    Register scene's events based functions.
    @param scene Pointer to the scene structure.
    @param type function's type.
    @param scene Pointer to the function.
    @return XNE_OK if the function is successfully linked, XNE_FAILTURE otherwise.
*/
int xne_scene_register_function(xne_Scene_t* scene, xne_SceneFunctionType_t type, void* func);

/*
    Add new lights to scene's lights.
    @param scene Pointer to the scene structure.
    @param type Type of the new lights (directional, point or spot)
    @return pointer to the new lights, if failed to allocate, return a NULL pointer.
*/
xne_Light_t* xne_scene_create_light(xne_Scene_t* scene, xne_LightType_t type);

void xne_destroy_scene(xne_Scene_t* scene);

#endif