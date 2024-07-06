#include "engine.h"

#include "graphics/device.h"

#include <assert.h>
#include <stdio.h>

static xne_Engine_t __instance;

xne_Engine_t* xne_create_engine_instance(){
    memset(&__instance, 0, sizeof(xne_Engine_t));
    __instance.state.late_time = xne_get_time();

    return &__instance;
}

xne_Engine_t* xne_get_engine_instance(){
    return &__instance;
}

void xne_destroy_engine_instance(){
    if(__instance.state.scene){
        xne_destroy_scene(__instance.state.scene);
    }
}

void xne_ask_init(){
    if(!__instance.state.scene->memory.size){
        fprintf(stderr, "current scene's memory isn't allocated!\n");
        return;
    }

    __instance.state.scene->initialization_func(__instance.state.scene);
}

void xne_ask_update(){
    __instance.state.scene->update_func(__instance.state.scene, __instance.state.delta_time);
}

void xne_ask_draw(){
    __instance.state.scene->draw_func(__instance.state.scene);
}

void xne_ask_destroy(){
    __instance.state.scene->destroy_func(__instance.state.scene);
}  