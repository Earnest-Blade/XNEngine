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

    xne_get_engine_instance()->state.scene = scene;

    return XNE_OK;
}

int xne_scene_register_function(xne_Scene_t* scene, xne_SceneFunctionType_t type, void* func){
    assert(scene);

    switch (type)
    {
    case XNE_UPDATE_FUNC:
        scene->update_func = func;
        return XNE_OK;
    
    case XNE_DRAW_FUNC:
        scene->draw_func = func;
        return XNE_OK;

    default:
        return XNE_FAILURE;
    }
}


void xne_destroy_scene(xne_Scene_t* scene){
    free(scene->name);
}

xne_Engine_t* xne_create_engine_instance(){
    memset(&__instance, 0, sizeof(xne_Engine_t));

    return &__instance;
}

xne_Engine_t* xne_get_engine_instance(){
    return &__instance;
}

void xne_destroy_engine_instance(){
    
}