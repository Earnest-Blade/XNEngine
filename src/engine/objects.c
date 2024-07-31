#include "objects.h"

#include "graphics/graphics.h"

#include <malloc.h>
#include <string.h>

// crapy macro 
// TODO: should I find a replacer ? or a more elegent way of doing this shit ?
#define XNE__FIND_MAT_UNIFORM_HOLDER(x, y) { if(strcmp(uniform_descriptor[y].name, #x) == 0) { material->uniforms.x = y; } }

int xne__create_material_from_object(json_object* root, void* dest){
    xne_Material_t* material = (xne_Material_t*) dest;

    const json_object* json_material = root;
    const json_object* json_shader = json_object_object_get(json_material, "Shader");
    const json_object* json_shaders = json_object_object_get(json_shader, "Shaders");
    const json_object* json_uniforms = json_object_object_get(json_shader, "Uniforms");

    material->uniforms.projection = XNE_INVALID_VALUE;
    material->uniforms.transform = XNE_INVALID_VALUE;
    material->uniforms.world = XNE_INVALID_VALUE;

    xne_ShaderDesc_t* shader_descriptor = (xne_ShaderDesc_t*) calloc(json_object_array_length(json_shaders), sizeof(xne_ShaderDesc_t));
    json_object* json_sub_shader;
    
    for (size_t i = 0; i < json_object_array_length(json_shaders); i++)
    {  
        json_sub_shader = json_object_array_get_idx(json_shaders, i);
        shader_descriptor[i].type = json_object_get_int(json_object_object_get(json_sub_shader, "Type"));
        shader_descriptor[i].name = json_object_get_string(json_object_object_get(json_sub_shader, "Name"));
    }

    if(xne_create_shader(&material->shader, json_object_get_string(json_object_object_get(json_shader, "Path")), shader_descriptor) == XNE_FAILURE){
        free(shader_descriptor);
        return XNE_FAILURE;
    }

    free(shader_descriptor);

    const size_t uniform_count = json_object_array_length(json_uniforms);
    xne_ShaderUniformDesc_t* uniform_descriptor = (xne_ShaderUniformDesc_t*) calloc(uniform_count + 1, sizeof(xne_ShaderUniformDesc_t));
    json_object* json_sub_uniform;
    
    for (size_t i = 0; i < uniform_count; i++)
    {
        json_sub_uniform = json_object_array_get_idx(json_uniforms, i);
        uniform_descriptor[i].attrib = (xne_UniformAttrib_t) json_object_get_int(json_object_object_get(json_sub_uniform, "Attribute"));
        uniform_descriptor[i].format = (xne_UniformType_t) json_object_get_int(json_object_object_get(json_sub_uniform, "Format"));
        uniform_descriptor[i].name = json_object_get_string(json_object_object_get(json_sub_uniform, "Name"));
        uniform_descriptor[i].length = XNE_INVALID_VALUE;

        xne_vprintf("%d - %s", i, uniform_descriptor[i].name);

        XNE__FIND_MAT_UNIFORM_HOLDER(projection, i);
        XNE__FIND_MAT_UNIFORM_HOLDER(transform, i);
        XNE__FIND_MAT_UNIFORM_HOLDER(world, i);

        if( (uniform_descriptor[i].attrib & XNE_UNIFORM_ATTRIB_ARRAY) || 
            (uniform_descriptor[i].attrib & XNE_UNIFORM_ATTRIB_STRUCT)) {

            const json_object* json_length_uniform = json_object_object_get(json_sub_uniform, "Length");
            if(json_object_get_type(json_length_uniform) != json_type_null){
                uniform_descriptor[i].length = json_object_get_int(json_length_uniform);
            }
            else {
                xne_printf("missing length uniform (due to attrib of type XNE_UNIFORM_ATTRIB_ARRAY and/or XNE_UNIFORM_ATTRIB_STRUCT)!");
            }
        }
    }

    memset(&uniform_descriptor[uniform_count], 0, sizeof(xne_ShaderUniformDesc_t));

    xne_link_shader_uniforms(&material->shader, uniform_descriptor);
    free(uniform_descriptor);
    
    xne__object_create_vec4(json_object_object_get(json_material, "AmbientColor"), material->ambient_color);
    xne__object_create_vec4(json_object_object_get(json_material, "DiffuseColor"), material->diffuse_color);

    material->ambient_texture = NULL;
    material->diffuse_texture = NULL;
}