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

#include "scene.h"
#include "graphics/device.h"
#include "graphics/graphics.h"

typedef struct xne_RuntimeState {
    double delta_time, late_time, interop;
    size_t frames;
    xne_Scene_t* scene;
} xne_RuntimeState_t;

typedef struct xne_Engine {
    xne_RuntimeState_t state;
    xne_Device_t device;
    xne_GraphicDevice_t graphics;
} xne_Engine_t;

xne_Engine_t* xne_create_engine_instance();
xne_Engine_t* xne_get_engine_instance();
void xne_destroy_engine_instance();

void xne_ask_init();
void xne_ask_update();
void xne_ask_draw();
void xne_ask_destroy();

#endif