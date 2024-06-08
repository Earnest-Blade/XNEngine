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

#include "core/core.h"

#include "graphics/camera.h"

typedef enum xne_SceneFunctionType {
    XNE_UPDATE_FUNC,
    XNE_DRAW_FUNC
} xne_SceneFunctionType_t;

typedef struct xne_Scene {
    char* name;

    xne_Camera_t camera;

    void (*update_func)(struct xne_Scene*, float);
    void (*draw_func)(struct xne_Scene*);
} xne_Scene_t;

int xne_create_scene(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera);
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