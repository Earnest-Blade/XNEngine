#include "engine.h"

#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>

#include "graphics/device.h"

static xne_Engine_t __instance;

int xne_create_scene(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera){
    assert(scene);

    memset(scene, 0, sizeof(xne_Scene_t));
    scene->name = (char*) malloc(strlen(name) + 1);
    strcpy(scene->name, name);

    xne_create_camera(&scene->camera, *camera);

    xne_get_engine_instance()->state.scene = scene;

    return XNE_OK;
}

void* xne_scene_allocate_buffer(xne_Scene_t* scene, size_t size, void* value){
    assert(scene);

    if(!scene->memory.size){
        scene->memory.size = size;
        scene->memory.elemsize = size;

        scene->memory.ptr = malloc(size);
        if(!scene->memory.ptr){
            fprintf(stderr, "failed to allocate a new memory chunk!\n");
            return NULL;
        }

        memset(scene->memory.ptr, 0, size);

        if(value){
            memcpy(scene->memory.ptr, value, size);
        }
    }
    else {
        fprintf(stderr, "cannot allocate a new buffer to the scene!\n");
        return NULL;
    }

    return scene->memory.ptr;
}

void* xne_scene_get_buffer(xne_Scene_t* scene){
    assert(scene);
    return scene->memory.ptr;
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

    if(scene->memory.size){
        free(scene->memory.ptr);
    }

    scene->update_func = NULL;
    scene->draw_func = NULL;
}

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