#pragma once

#ifndef XNE_ENGINE_IMPL
#define XNE_ENGINE_IMPL

#ifndef XNE_TARGET_FPS
    #define XNE_TARGET_FPS (int)(60)
#endif

#ifndef XNE_OPTIMAL_TIME
    #define XNE_OPTIMAL_TIME (long)(1000 / XNE_TARGET_FPS)
#endif

#ifndef XNE_MAX_SKIP_FRAME
    #define XNE_MAX_SKIP_FRAME 5
#endif

#define XNE_CORE_BUFFER
#include "core/core.h"

#include "graphics/camera.h"

typedef enum xne_SceneFunctionType {
    XNE_UPDATE_FUNC,
    XNE_DRAW_FUNC
} xne_SceneFunctionType_t;

typedef struct xne_Scene {
    char* name;
    struct xne_Buffer memory;
    xne_Camera_t camera;

    void (*update_func)(struct xne_Scene*, float);
    void (*draw_func)(struct xne_Scene*);
} xne_Scene_t;

/*
    Create a new scene into a previous allocated struct.
    @param scene Pointer to the scene structure.
    @param name Scene's name.
    @param camera Camera descriptor structure use to create the default scene's camera.
    @return XNE_OK if the scene is successfully created, XNE_FAILTURE otherwise.
*/
int xne_create_scene(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera);

/*
    Allocate a new memory chunck use to store scene's variables. 
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
    @return Pointer to the scene's memory buffer if it exists, a null pointer otherwise.
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


void xne_destroy_scene(xne_Scene_t* scene);

typedef struct xne_RuntimeState {
    double delta_time, late_time, interop;
    size_t frames;
    xne_Scene_t* scene;
} xne_RuntimeState_t;

typedef struct xne_Engine {
    xne_RuntimeState_t state;
} xne_Engine_t;

xne_Engine_t* xne_create_engine_instance();
xne_Engine_t* xne_get_engine_instance();

void xne_destroy_engine_instance();

#endif