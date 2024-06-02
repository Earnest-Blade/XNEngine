#include "model.h"
#include "objects.h" 
#include "material.h"

#include "engine/engine.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"

#include <cglm/cglm.h>

#include <assert.h>
#include <string.h>
#include <memory.h>

static void xne__model_create_node(json_object* json, xne_Model_t* model, xne_Tree_t* tree){
    assert(json);
    assert(model);
    assert(tree);

    xne_ModelNode_t* node = (xne_ModelNode_t*) xne_tree_get_value(tree);
    assert(node);

    const json_object* childs = json_object_object_get(json, "Childs");
    const json_object* mesh = json_object_object_get(json, "Mesh");
    const json_object* material = json_object_object_get(json, "Material");
    const json_object* transform = json_object_object_get(json, "Transform");
    json_object* name = json_object_object_get(json, "Name");
        
    xne_create_transform(&node->transform);
    if(json_object_get_type(transform) != json_type_null){
        vec3 position, scale;
        vec4 rotation;
        xne__object_create_vec3(json_object_object_get(transform, "Position"), position);
        xne__object_create_vec3(json_object_object_get(transform, "Scale"), scale);
        xne__object_create_quat(json_object_object_get(transform, "Rotation"), rotation);

        glm_vec3_copy(position, node->transform.position);
        glm_vec3_copy(scale, node->transform.scale);
        glm_vec4_copy(rotation, node->transform.rotation);
        xne_transform_moveto(&node->transform, 0, 0, 0);
    }

    node->mesh = NULL;
    node->material = NULL;
    node->name = (char*) malloc(json_object_get_string_len(name) + 1);
    strcpy(node->name, json_object_get_string(name));
    
    if(json_object_get_type(mesh) != json_type_null){
        node->mesh = (xne_Mesh_t*) xne_vector_get(&model->meshes, json_object_get_uint64(mesh));
    }

    if(json_object_get_type(material) != json_type_null){
        node->material = (xne_Material_t*) xne_vector_get(&model->materials, json_object_get_uint64(material));
    }

    xne_tree_fixed_childrens(tree, json_object_array_length(childs));
    for (size_t i = 0; i < tree->child_count; i++)
    {
        xne__model_create_node(json_object_array_get_idx(childs, i), model, xne_tree_get_child(tree, i));
    }
}

static void xne__model_world_mat(xne_ModelNode_t* node, xne_Tree_t* tree){
    if(xne_tree_is_root(tree)){
        glm_mat4_identity(node->transform.world);
    }

    xne__model_world_mat((xne_ModelNode_t*)xne_tree_get_value(tree->parent), tree->parent);
    assert(0);
}

static void xne__model_draw_node(xne_Tree_t* tree){
    if(!tree) return;

    for (size_t i = 0; i < tree->child_count; i++)
    {
        xne__model_draw_node(xne_tree_get_child(tree, i));
    }

    xne_ModelNode_t* node = (xne_ModelNode_t*) xne_tree_get_value(tree);
    if(!node || !node->mesh || !node->material) return;

    xne_shader_enable(&node->material->shader);
    
    xne_shader_use_uniform(&node->material->shader, 0, xne_get_camera_projection(&xne_get_engine_instance()->state.scene->camera));
    xne_shader_use_uniform(&node->material->shader, 1, xne_transform_matrix(&node->transform));

    // in order to keep an ordered unit, units aren't fixed but dynamics.
    // So, if a texture isn't use, it's unit will be use by the next texture.
    uint32_t units = 0;

    if(node->material->ambient_texture){
        xne_texture_enable(node->material->ambient_texture, (xne_TextureUnit_t)(XNE_TEXTURE_UNIT0 + units));
        units++;
    }
    
    if(node->material->diffuse_texture){
        xne_texture_enable(node->material->diffuse_texture, (xne_TextureUnit_t)(XNE_TEXTURE_UNIT0 + units));
        units++;
    }
    
    xne_draw_mesh(node->mesh);

    xne_shader_disable(NULL);
}

static void xne__model_destroy_node(xne_Tree_t* tree){
    if(!tree) return;

    for (size_t i = 0; i < tree->child_count; i++)
    {
        xne__model_destroy_node(xne_tree_get_child(tree, i));
    }

    xne_ModelNode_t* node = (xne_ModelNode_t*) tree->memory.ptr;
    free(node->name);
}

int xne_create_modelf(xne_Model_t* model, FILE* file){
    assert(model);
    assert(file);

    memset(model, 0, sizeof(xne_Model_t));

    fseek(file, 0, SEEK_SET);
    char header = xne_freadw8(file);

    size_t fsize = xne_fsize(file);
    char* fstr = (char*) malloc(fsize + 1);
    size_t fsize0 = fread(fstr, sizeof(char), fsize, file);
    fstr[fsize0] = '\0';

    if(fsize > fsize0 + 1){
        fprintf(stderr, "could not load the entire file!\n");
        free(fstr);

        return XNE_FAILURE;
    }

    if(header){
        xne_inflate(&fstr, &fsize);
        fstr[fsize] = '\0';
    }

    const xne_VertexAlignDesc_t vertex_align[] = {
        { XNE_VERTEX_POSITION, XNE_FLOAT, offsetof(xne_Vertex_t, position) },
        { XNE_VERTEX_TEXCOORD, XNE_FLOAT, offsetof(xne_Vertex_t, uv) },
        { XNE_VERTEX_NORMAL, XNE_FLOAT, offsetof(xne_Vertex_t, normal)},
        { XNE_VERTEX_ATTRIB_END, XNE_FLOAT, sizeof(xne_Vertex_t) }
    };

    __json_context = json_tokener_parse(fstr);
    if(!__json_context){
        fprintf(stderr, "failed to parse file!\n");
        free(fstr);

        return XNE_FAILURE;
    }

    const json_object* json_scene = json_object_object_get(json_object_object_get(__json_context, "Asset"), "Value");
    if(json_object_get_type(json_scene) == json_type_null){
        fprintf(stderr, "cannot find scene object!\n");
        json_object_put(__json_context);
        free(fstr);

        return XNE_FAILURE;
    }

    const json_object* json_mesh_array = json_object_object_get(json_scene, "Meshes");
    const json_object* json_texture_array = json_object_object_get(json_scene, "Textures");
    const json_object* json_materials_array = json_object_object_get(json_scene, "Materials");

    if(json_object_get_type(json_mesh_array) != json_type_null){
        xne_create_vector(&model->meshes, sizeof(xne_Mesh_t), json_object_array_length(json_mesh_array));
        model->meshes.count = model->meshes.capacity;

        json_object* json_mesh, *json_vertices, *json_elements;
        for (size_t i = 0; i < model->meshes.count; i++)
        {
            json_mesh = json_object_array_get_idx(json_mesh_array, i);
            assert(json_mesh); // @TODO: replace with return XNE_FAILTURE type

            json_vertices = json_object_object_get(json_mesh, "Vertices");
            json_elements = json_object_object_get(json_mesh, "Elements");
            const size_t vertices_count = json_object_array_length(json_vertices);
            const size_t elements_count = json_object_array_length(json_elements);

            float* vertices = (float*) malloc(vertices_count * sizeof(float));
            uint32_t* elements = (uint32_t*) malloc(elements_count * sizeof(uint32_t));

            for (size_t y = 0; y < vertices_count; y++)
            {
                vertices[y] = (float) json_object_get_double(json_object_array_get_idx(json_vertices, y));
            }

            for (size_t y = 0; y < elements_count; y++)
            {
                elements[y] = (uint32_t) json_object_get_uint64(json_object_array_get_idx(json_elements, y));
            }
            
            xne_MeshDesc_t mesh_desc;
            mesh_desc.vertices = vertices;
            mesh_desc.vertex_align = vertex_align;
            mesh_desc.elements = elements;
            mesh_desc.vertices_count = vertices_count / (sizeof(xne_Vertex_t) / 4);
            mesh_desc.elements_count = elements_count;
            mesh_desc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;

            xne_create_mesh((xne_Mesh_t*)xne_vector_get(&model->meshes, i), mesh_desc);
            free(vertices);
            free(elements);
        }
    }
    else {
        fprintf(stderr, "cannot find mesh array object!\n");
        free(fstr);
        json_object_put(__json_context);

        return XNE_FAILURE;
    }

    if(json_object_get_type(json_texture_array) != json_type_null){
        xne_create_vector(&model->textures, sizeof(xne_Texture_t), json_object_array_length(json_texture_array));
        model->textures.count = model->textures.capacity;

        json_object* json_texture;
        for (size_t i = 0; i < model->textures.count; i++)
        {
            json_texture = json_object_array_get_idx(json_texture_array, i);
            assert(json_texture);

            xne_create_texture(
                (xne_Texture_t*) xne_vector_get(&model->textures, i),
                json_object_get_string(json_object_object_get(json_texture, "Path")),
                (xne_TextureFilter_t) json_object_get_int(json_object_object_get(json_texture, "Filter")),
                (xne_TextureWrap_t) json_object_get_int(json_object_object_get(json_texture, "Wrap"))
            );
        }
    }
    else {
        fprintf(stderr, "cannot find texture array object!\n");
        free(fstr);
        json_object_put(__json_context);

        return XNE_FAILURE;
    }

    if(json_object_get_type(json_materials_array) != json_type_null){
        xne_create_vector(&model->materials, sizeof(xne_Material_t), json_object_array_length(json_materials_array));
        model->materials.count = model->materials.capacity;

        xne_Material_t* material = NULL;
        json_object* json_material, *json_shader, *json_shaders, *json_uniforms;
        for (size_t i = 0; i < model->materials.count; i++)
        {
            json_material = json_object_array_get_idx(json_materials_array, i);
            json_shader = json_object_object_get(json_material, "Shader");
            json_shaders = json_object_object_get(json_shader, "Shaders");
            json_uniforms = json_object_object_get(json_shader, "Uniforms");

            material = (xne_Material_t*) xne_vector_get(&model->materials, i);

            xne_ShaderDesc_t* shader_desc = (xne_ShaderDesc_t*) calloc(json_object_array_length(json_shaders), sizeof(xne_ShaderDesc_t));
            for (size_t y = 0; y < json_object_array_length(json_shaders); y++)
            {
                json_object* json_sub_shader = json_object_array_get_idx(json_shaders, y);
                shader_desc[y].type = json_object_get_int(json_object_object_get(json_sub_shader, "Type"));
                shader_desc[y].name = json_object_get_string(json_object_object_get(json_sub_shader, "Name"));
            }

            xne_create_shader(
                &material->shader,
                json_object_get_string(json_object_object_get(json_shader, "Path")),
                shader_desc
            );
            free(shader_desc);

            xne_ShaderUniformDesc_t* uniform_desc = (xne_ShaderUniformDesc_t*) calloc(json_object_array_length(json_uniforms), sizeof(xne_ShaderUniformDesc_t));
            for (size_t y = 0; y < json_object_array_length(json_uniforms); y++)
            {
                json_object* suniform = json_object_array_get_idx(json_uniforms, y);
                uniform_desc[y].attrib = json_object_get_int(json_object_object_get(suniform, "Attribute"));
                uniform_desc[y].format = (xne_UniformType_t) json_object_get_int(json_object_object_get(suniform, "Format"));
                uniform_desc[y].name = json_object_get_string(json_object_object_get(suniform, "Name"));
            }

            xne_link_shader_uniforms(&material->shader, uniform_desc);
            free(uniform_desc);

            xne__object_create_vec4(json_object_object_get(json_material, "AmbientColor"), material->ambient_color);
            xne__object_create_vec4(json_object_object_get(json_material, "DiffuseColor"), material->diffuse_color);

            material->ambient_texture = NULL;
            material->diffuse_texture = NULL;

            if(json_object_get_type(json_object_object_get(json_material, "AmbientTexture")) != json_type_null){
                material->ambient_texture = (xne_Texture_t*) xne_vector_get(&model->textures, json_object_get_int(json_object_object_get(json_material, "AmbientTexture")));
                xne_link_texture(
                    material->ambient_texture,
                    json_object_get_string(json_object_object_get(json_object_array_get_idx(json_texture_array, json_object_get_int(json_object_object_get(json_material, "AmbientTexture"))), "Name")), 
                    material->shader.program
                );
            }

            if(json_object_get_type(json_object_object_get(json_material, "DiffuseTexture")) != json_type_null){
                material->diffuse_texture = (xne_Texture_t*) xne_vector_get(&model->textures, json_object_get_int(json_object_object_get(json_material, "DiffuseTexture")));
                xne_link_texture(
                    material->diffuse_texture,
                    json_object_get_string(json_object_object_get(json_object_array_get_idx(json_texture_array, json_object_get_int(json_object_object_get(json_material, "DiffuseTexture"))), "Name")), 
                    material->shader.program
                );
            }
        }
    }
    else {
        fprintf(stderr, "cannot find texture material object!\n");
        free(fstr);
        json_object_put(__json_context);

        return XNE_FAILURE;
    }

    xne_create_tree(&model->root, NULL, sizeof(xne_ModelNode_t));
    xne__model_create_node(json_object_object_get(json_scene, "Root"), model, &model->root);

    free(fstr);
    json_object_put(__json_context);
    
    return XNE_OK;
}

void xne_draw_model(xne_Model_t* model){
    xne__model_draw_node(&model->root);
}

void xne_destroy_model(xne_Model_t* model){
    xne__model_destroy_node(&model->root);
    xne_destroy_tree(&model->root);

    xne_destroy_vector(&model->meshes);
    xne_destroy_vector(&model->textures);
    xne_destroy_vector(&model->materials);
}