#include "model.h"

#include "engine/engine.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/material.h"

#include "objects.h" 

#include <assert.h>
#include <string.h>
#include <memory.h>

// crapy macro 
// TODO: should I find a replacer ? or a more elegent way of doing this shit ?
#define XNE__FIND_MAT_UNIFORM_HOLDER(x, y) { if(strcmp(uniform_desc[y].name, #x) == 0) { material->uniforms.x = y; } }

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

    if(!xne_tree_is_root(tree)){
        node->transform.parent = &((xne_ModelNode_t*)xne_tree_get_value(tree->parent))->transform;
    }

    if(json_object_get_type(transform) != json_type_null){
        xne_vec3 position, scale;
        xne_vec4 rotation;
        xne__object_create_vec3(json_object_object_get(transform, "Position"), position);
        xne__object_create_vec3(json_object_object_get(transform, "Scale"), scale);
        xne__object_create_quat(json_object_object_get(transform, "Rotation"), rotation);

        memcpy(node->transform.position, position, 3 * sizeof(float));
        memcpy(node->transform.scale, scale, 3 * sizeof(float));
        memcpy(node->transform.rotation, rotation, 4 * sizeof(float));
        xne_transform_move_to(&node->transform, 0, 0, 0);
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

static void xne__model_draw_node(xne_Tree_t* tree){
    if(!tree) return;

    for (size_t i = 0; i < tree->child_count; i++)
    {
        xne__model_draw_node(xne_tree_get_child(tree, i));
    }

    xne_ModelNode_t* node = (xne_ModelNode_t*) xne_tree_get_value(tree);
    if(!node || !node->mesh || !node->material) return;

    xne_shader_enable(&node->material->shader);
    
    xne_shader_use_uniform(&node->material->shader, node->material->uniforms.projection, xne_get_camera_projection(&xne_get_engine_instance()->state.scene->camera));
    xne_shader_use_uniform(&node->material->shader, node->material->uniforms.transform, xne_transform_matrix(&node->transform));
    xne_shader_use_uniform(&node->material->shader, node->material->uniforms.world, xne_transform_world_matrix(&node->transform));

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
    if(node){
        free(node->name);
    }
}

int xne_create_modelf(xne_Model_t* model, FILE* file){
    assert(model);
    assert(file);

    memset(model, 0, sizeof(xne_Model_t));

    // read the hole file
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

    // uncompress
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

    // open json parser
    __json_context = json_tokener_parse(fstr);
    if(!__json_context){
        fprintf(stderr, "failed to parse file!\n");
        free(fstr);

        return XNE_FAILURE;
    }

    // check if this is the correct type
    if(xne__object_is_type_of(__json_context, XNE_OBJECT_MODEL) != XNE_OK){
        fprintf(stderr, "object's type is incorrect or is corrupted!\n");
        json_object_put(__json_context);
        free(fstr);
        return XNE_FAILURE;
    }

    // root point of the data
    const json_object* json_data_holder = json_object_object_get(json_object_object_get(__json_context, "Asset"), "Value");

    // initialize root of each types
    const json_object* json_mesh_array = json_object_object_get(json_data_holder, "Meshes");
    const json_object* json_texture_array = json_object_object_get(json_data_holder, "Textures");
    const json_object* json_materials_array = json_object_object_get(json_data_holder, "Materials");

    // create and allocate meshes datas
    if(json_object_get_type(json_mesh_array) != json_type_null){
        xne_create_vector(&model->meshes, sizeof(xne_Mesh_t), json_object_array_length(json_mesh_array));
        model->meshes.count = model->meshes.capacity;

        json_object* json_mesh, *json_vertices, *json_elements;
        for (size_t i = 0; i < model->meshes.count; i++)
        {
            json_mesh = json_object_array_get_idx(json_mesh_array, i);
            assert(json_mesh); // @TODO: replace with return XNE_FAILTURE type

            // read vertices and elements buffers
            json_vertices = json_object_object_get(json_mesh, "Vertices");
            json_elements = json_object_object_get(json_mesh, "Elements");
            const size_t vertices_count = json_object_array_length(json_vertices);
            const size_t elements_count = json_object_array_length(json_elements);

            float* vertices = (float*) malloc(vertices_count * sizeof(float));
            uint32_t* elements = (uint32_t*) malloc(elements_count * sizeof(uint32_t));

            // copy each vertices
            for (size_t y = 0; y < vertices_count; y++)
            {
                vertices[y] = (float) json_object_get_double(json_object_array_get_idx(json_vertices, y));
            }

            // copy each elements
            for (size_t y = 0; y < elements_count; y++)
            {
                elements[y] = (uint32_t) json_object_get_uint64(json_object_array_get_idx(json_elements, y));
            }

            // create the new mesh            
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

    // initialize and allocate textures
    if(json_object_get_type(json_texture_array) != json_type_null){
        xne_create_vector(&model->textures, sizeof(xne_Texture_t), json_object_array_length(json_texture_array));
        model->textures.count = model->textures.capacity;

        json_object* json_texture;
        for (size_t i = 0; i < model->textures.count; i++)
        {
            // simply load each textures
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

    fprintf(stdout, "finised loading model!\n");

    // initialize and allocate materials and shaders
    if(json_object_get_type(json_materials_array) != json_type_null){
        xne_create_vector(&model->materials, sizeof(xne_Material_t), json_object_array_length(json_materials_array));
        model->materials.count = model->materials.capacity;

        xne_Material_t* material = NULL;
        json_object* json_material, *json_shader, *json_shaders, *json_uniforms;
        for (size_t i = 0; i < model->materials.count; i++)
        {
            // get json elements
            json_material = json_object_array_get_idx(json_materials_array, i);
            material = (xne_Material_t*) xne_vector_get(&model->materials, i);

            xne__create_material_from_object(json_material, material);

            /*json_shader = json_object_object_get(json_material, "Shader");
            json_shaders = json_object_object_get(json_shader, "Shaders");
            json_uniforms = json_object_object_get(json_shader, "Uniforms");

            // create an empty material struct
            material = (xne_Material_t*) xne_vector_get(&model->materials, i);

            material->uniforms.projection = XNE_INVALID_VALUE;
            material->uniforms.transform = XNE_INVALID_VALUE;
            material->uniforms.world = XNE_INVALID_VALUE;

            // load shader's sub shaders
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

            // load shader's uniforms
            const size_t uniform_count = json_object_array_length(json_uniforms);
            xne_ShaderUniformDesc_t* uniform_desc = (xne_ShaderUniformDesc_t*) calloc(uniform_count + 1, sizeof(xne_ShaderUniformDesc_t));

            for (size_t y = 0; y < uniform_count; y++)
            {
                json_object* suniform = json_object_array_get_idx(json_uniforms, y);
                uniform_desc[y].attrib = (xne_UniformAttrib_t) json_object_get_int(json_object_object_get(suniform, "Attribute"));
                uniform_desc[y].format = (xne_UniformType_t) json_object_get_int(json_object_object_get(suniform, "Format"));
                uniform_desc[y].name = json_object_get_string(json_object_object_get(suniform, "Name"));
                uniform_desc[y].length = XNE_INVALID_VALUE;

                xne_vprintf("%d - %s", y, uniform_desc[y].name);

                XNE__FIND_MAT_UNIFORM_HOLDER(projection, y);
                XNE__FIND_MAT_UNIFORM_HOLDER(transform, y);
                XNE__FIND_MAT_UNIFORM_HOLDER(world, y);

                if(uniform_desc[y].attrib & XNE_UNIFORM_ATTRIB_ARRAY) {
                    const json_object* json_length_uniform = json_object_object_get(suniform, "Length");
                    if(json_object_get_type(json_length_uniform) != json_type_null){
                        uniform_desc[y].length = json_object_get_int(json_length_uniform);
                    }
                    else {
                        xne_printf("missing length uniform (due to attrib of type XNE_UNIFORM_ATTRIB_ARRAY)!");
                    }
                }
            }

            memset(&uniform_desc[uniform_count], 0, sizeof(xne_ShaderUniformDesc_t));

            xne_link_shader_uniforms(&material->shader, uniform_desc);
            free(uniform_desc);

            // load materials colors
            xne__object_create_vec4(json_object_object_get(json_material, "AmbientColor"), material->ambient_color);
            xne__object_create_vec4(json_object_object_get(json_material, "DiffuseColor"), material->diffuse_color);

            // load materials textures
            material->ambient_texture = NULL;
            material->diffuse_texture = NULL;*/

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

    // initialize the tree and create each node with recursives calls
    xne_create_tree(&model->root, NULL, sizeof(xne_ModelNode_t));
    xne__model_create_node(json_object_object_get(json_data_holder, "Root"), model, &model->root);

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

    for (size_t i = 0; i < model->meshes.count; i++)
    {
        xne_destroy_mesh((xne_Mesh_t*) xne_vector_get(&model->meshes, i));
    }

    for (size_t i = 0; i < model->textures.count; i++)
    {
        xne_destroy_texture((xne_Texture_t*) xne_vector_get(&model->textures, i));
    }

    for (size_t i = 0; i < model->materials.count; i++)
    {
        xne_destroy_material((xne_Material_t*) xne_vector_get(&model->materials, i));
    }
    
    xne_destroy_vector(&model->meshes);
    xne_destroy_vector(&model->textures);
    xne_destroy_vector(&model->materials);
}