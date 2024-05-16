#include "engine.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>

static xne_Engine_t __instance;

int xne_create_scene(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera){
    assert(scene);

    memset(scene, 0, sizeof(xne_Scene_t));
    scene->name = (char*) malloc(strlen(name));
    strcpy(scene->name, name);

    xne_create_camera(&scene->camera, *camera);

    return XNE_OK;
}

void xne_destroy_scene(xne_Scene_t* scene){
    free(scene->name);
}

int xne_create_engine_instance(){
    __instance.state;
    __instance.state.late_time = 0.0f;
    __instance.state.delta_time = 0.0f;
}

xne_Engine_t* xne_get_engine_instance(){
    return &__instance;
}

void xne_destroy_engine_instance(){

}