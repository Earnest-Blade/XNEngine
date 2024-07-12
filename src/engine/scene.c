#include "scene.h"

#include "engine.h"
#include "objects.h"

#include <stdio.h>
#include <assert.h>

int xne_create_scened(xne_Scene_t* scene, const char* name, xne_Camera_Desc_t* camera){
    assert(scene);

    memset(scene, 0, sizeof(xne_Scene_t));
    
    if(name){
        scene->name = (char*) malloc(strlen(name) + 1);
        strcpy(scene->name, name);
    }
    
    if(camera){
        xne_create_camera(&scene->camera, *camera);
    }

    xne_create_vector(&scene->point_lights, sizeof(xne_Light_t), 1);
    xne_create_vector(&scene->spot_lights, sizeof(xne_Light_t), 1);
    xne_get_engine_instance()->state.scene = scene;

    return XNE_OK;
}

int xne_create_scenef(xne_Scene_t* scene, FILE* file){
    assert(scene);
    assert(file);

    memset(scene, 0, sizeof(xne_Scene_t));

    fseek(file, 0, SEEK_SET);
    char header = xne_freadw8(file);

    size_t fsize = xne_fsize(file);
    char* fstr = (char*) malloc(fsize + 1);
    size_t fsize0 = fread(fstr, sizeof(char), fsize, file);

    if(fsize > fsize0 + 1){
        fprintf(stderr, "could not load the entire file!\n");
        free(fstr);

        return XNE_FAILURE;
    }

    // uncompress
    if(header){
        xne_inflate(&fstr, &fsize);
        fstr[fsize] = '\0';
    }

    // open json parser context
    __json_context = json_tokener_parse(fstr);
    if(!__json_context){
        xne_printf("failed to parse file!");
        free(fstr);

        return XNE_FAILURE;
    }

    if(xne__object_is_type_of(__json_context, XNE_OBJECT_SCENE) != XNE_OK){
        xne_printf("object's type is incorrect or is corrupted !");
        json_object_put(__json_context);
        free(fstr);
        return XNE_FAILURE;
    }

    const json_object* json_scene = json_object_object_get(json_object_object_get(__json_context, "Asset"), "Value");

    const json_object* json_camera = json_object_object_get(json_scene, "Camera");
    const json_object* json_lights = json_object_object_get(json_scene, "Lights");
    
    if(json_object_get_type(json_camera) != json_type_null){
        xne_Camera_Desc_t camera_desc;
        camera_desc.projection = json_object_get_int(json_object_object_get(json_camera, "Projection"));
        camera_desc.fov = json_object_get_double(json_object_object_get(json_camera, "Fov"));
        camera_desc.far = json_object_get_double(json_object_object_get(json_camera, "Far"));
        camera_desc.near = json_object_get_double(json_object_object_get(json_camera, "Near"));
        
        camera_desc.width = xne_get_engine_instance()->graphics.draw.framebuffer.width;
        camera_desc.height = xne_get_engine_instance()->graphics.draw.framebuffer.height;
        xne_create_camera(&scene->camera, camera_desc);
    }
    else {
        xne_printf("cannot find camera's node !");
        json_object_put(__json_context);
        free(fstr);
        return XNE_FAILURE;
    }

    if(json_object_get_type(json_lights) != json_type_null) {
        const json_object* json_dir_light = json_object_object_get(json_lights, "DirectionalLight");

        if(json_object_get_type(json_dir_light) != json_type_null){
            scene->directional_light.type = XNE_LIGHT_DIRECTIONAL;
            
            xne__object_create_vec3(json_object_object_get(json_dir_light, "Position"), scene->directional_light.position);
            xne__object_create_vec3(json_object_object_get(json_dir_light, "Direction"), scene->directional_light.light.directional.direction);
            xne__object_create_vec3(json_object_object_get(json_dir_light, "Color"), scene->directional_light.light.directional.color);
        }
    }
    
    xne_create_vector(&scene->point_lights, sizeof(xne_Light_t), 1);
    xne_create_vector(&scene->spot_lights, sizeof(xne_Light_t), 1);
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
            xne_printf("failed to allocate a new memory chunk!");
            return NULL;
        }

        memset(scene->memory.ptr, 0, size);
        if(value){
            memcpy(scene->memory.ptr, value, size);
        }
    }
    else {
        xne_printf("cannot allocate a new buffer to the scene!");
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

xne_Light_t* xne_scene_create_light(xne_Scene_t* scene, xne_LightType_t type){
    xne_Light_t* light;

    switch (type)
    {
    case XNE_LIGHT_DIRECTIONAL:
        scene->directional_light.type = type; 
        return &scene->directional_light;
    
    case XNE_LIGHT_POINT:
        light = (xne_Light_t*) xne_vector_push_back(&scene->point_lights, NULL);
        xne_create_light(light, type);
        return light;

    case XNE_LIGHT_SPOT:
        light = (xne_Light_t*) xne_vector_push_back(&scene->spot_lights, NULL);
        xne_create_light(light, type);
        return light; 

    default:
        break;
    }
}

void xne_destroy_scene(xne_Scene_t* scene){
    free(scene->name);

    if(scene->memory.size){
        free(scene->memory.ptr);
    }

    xne_destroy_vector(&scene->point_lights);
    xne_destroy_vector(&scene->spot_lights);

    scene->update_func = NULL;
    scene->draw_func = NULL;
    scene->initialization_func = NULL;
    scene->destroy_func = NULL;
}