#pragma once

#ifndef XNE_ENGINE_IMPL
#define XNE_ENGINE_IMPL

#include "core/core.h"

#include "graphics/camera.h"

typedef struct xne_Scene {
    char* name;

    xne_Camera_t camera;
} xne_Scene_t;

int xne_create_scene(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera);
void xne_destroy_scene(xne_Scene_t* scene);

typedef struct xne_RuntimeState {
    float delta_time, late_time;
    xne_Scene_t* scene;
} xne_RuntimeState_t;

typedef struct xne_Engine {
    xne_RuntimeState_t state;
} xne_Engine_t;

int xne_create_engine_instance();
xne_Engine_t* xne_get_engine_instance();
void xne_destroy_engine_instance();

#endif