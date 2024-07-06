#include "scene.h"

#include "engine.h"

#include <stdio.h>
#include <assert.h>

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
    case XNE_INIT_FUNC:
        scene->initialization_func = func;
        return XNE_OK;

    case XNE_UPDATE_FUNC:
        scene->update_func = func;
        return XNE_OK;
    
    case XNE_DRAW_FUNC:
        scene->draw_func = func;
        return XNE_OK;

    case XNE_DESTROY_FUNC:
        scene->destroy_func = func;

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
    scene->initialization_func = NULL;
    scene->destroy_func = NULL;
}