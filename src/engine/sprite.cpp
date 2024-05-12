#include "sprite.h"

#include <gl/glew.h>

#include "objects.h"

#include <memory.h>
#include <assert.h>

int xne_create_sprited(xne_Sprite_t* sprite, xne_SpriteDesc_t desc){
    assert(sprite);
    memset(sprite, 0, sizeof(xne_Sprite_t));

    sprite->width = desc.sprite_width;
    sprite->height = desc.sprite_height;

    const xne_VertexAlignDesc_t align[] = {
        {XNE_VERTEX_POSITION, XNE_FLOAT, 0},
        {XNE_VERTEX_TEXCOORD, XNE_FLOAT, 8},
        {XNE_VERTEX_ALIGN_END(16)}
    };

    const float vertices[16] = {
        -desc.sprite_width,  desc.sprite_height, 1, 0,
        -desc.sprite_width, -desc.sprite_height, 1, 1, 
         desc.sprite_width, -desc.sprite_height, 0, 1,
         desc.sprite_width,  desc.sprite_height, 0, 0
    };
    const uint32_t elements[6] = {0, 1, 2, 0, 2, 3};

    xne_MeshDesc_t mdesc = {};
    mdesc.vertices = vertices;
    mdesc.elements = elements;
    mdesc.vertex_align = align;
    mdesc.vertices_count = 4;
    mdesc.elements_count = 6;
    mdesc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;

    xne_create_mesh(&sprite->plane, mdesc);
    if(xne_create_shader(&sprite->shader, desc.shader, desc.shader_desc) != XNE_OK){
        fprintf(stderr, "failed to create sprite shader!");
        return XNE_FAILURE;
    }

    if(xne_link_shader_uniforms(&sprite->shader, desc.uniform_desc) != XNE_OK){
        fprintf(stderr, "failed to create sprite shader's uniforms!");
        return XNE_FAILURE;
    }

    xne_create_texture_atlas(&sprite->textures, desc.path, desc.tile_width_count, 
        desc.tile_height_count, desc.filter, desc.wrap);
    xne_link_texture_atlas(&sprite->textures, "texture0", sprite->shader.program);
    xne_create_transform(&sprite->transform);

    return XNE_OK;
}

int xne_create_spritef(xne_Sprite_t* sprite, FILE* file){
    assert(sprite);
    assert(file);

    memset(sprite, 0, sizeof(xne_Sprite_t));

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

    if(header){
        xne_inflate(&fstr, &fsize);
        fstr[fsize] = '\0';
    }

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

    const json_object* json_sprite = json_object_object_get(json_scene, "Sprite");
    const json_object* json_atlas = json_object_object_get(json_scene, "Atlas");
    const json_object* json_material = json_object_object_get(json_scene, "Material");

    if(json_object_get_type(json_sprite) != json_type_null){
        sprite->width = (float)json_object_get_double(json_object_object_get(json_sprite, "PlaneWidth"));
        sprite->height = (float)json_object_get_double(json_object_object_get(json_sprite, "PlaneHeight"));

        const xne_VertexAlignDesc_t align[] = {
            {XNE_VERTEX_POSITION, XNE_FLOAT, 0},
            {XNE_VERTEX_TEXCOORD, XNE_FLOAT, 8},
            {XNE_VERTEX_ALIGN_END(16)}
        };

        const float vertices[16] = {
            -sprite->width,  sprite->height, 1, 0,
            -sprite->width, -sprite->height, 1, 1, 
             sprite->width, -sprite->height, 0, 1,
             sprite->width,  sprite->height, 0, 0
        };
        const uint32_t elements[6] = {0, 1, 2, 0, 2, 3};
        
        xne_MeshDesc_t mesh_desc = {};
        mesh_desc.vertices = vertices;
        mesh_desc.elements = elements;
        mesh_desc.vertex_align = align;
        mesh_desc.vertices_count = 4;
        mesh_desc.elements_count = 6;
        mesh_desc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;

        xne_create_mesh(&sprite->plane, mesh_desc);
    }
    else{
        fprintf(stdout, "cannot find sprite object!\n");
        free(fstr);
        json_object_put(__json_context);

        return XNE_FAILURE;
    }

    if(json_object_get_type(json_material) != json_type_null){
        json_object* json_shader = json_object_object_get(json_material, "Shader");
        json_object* json_shaders = json_object_object_get(json_shader, "Shaders");
        json_object* json_uniforms = json_object_object_get(json_shader, "Uniforms");

        xne_ShaderDesc_t* shader_desc = (xne_ShaderDesc_t*) calloc(json_object_array_length(json_shaders), sizeof(xne_ShaderDesc_t));
        for (size_t y = 0; y < json_object_array_length(json_shaders); y++)
        {
            json_object* json_sub_shader = json_object_array_get_idx(json_shaders, y);
            shader_desc[y].type = json_object_get_int(json_object_object_get(json_sub_shader, "Type"));
            shader_desc[y].name = json_object_get_string(json_object_object_get(json_sub_shader, "Name"));
        }

        xne_create_shader(
            &sprite->shader,
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

        xne_link_shader_uniforms(&sprite->shader, uniform_desc);
        free(uniform_desc);
    }
    else{
        fprintf(stdout, "cannot find material object!\n");
        free(fstr);
        json_object_put(__json_context);
        
        return XNE_FAILURE;
    }

    if(json_object_get_type(json_atlas) != json_type_null){
        xne_create_texture_atlas(
            &sprite->textures,
            json_object_get_string(json_object_object_get(json_atlas, "Path")),
            json_object_get_int(json_object_object_get(json_atlas, "TileWidthCount")),
            json_object_get_int(json_object_object_get(json_atlas, "TileHeightCount")),
            (xne_TextureFilter_t)json_object_get_int(json_object_object_get(json_atlas, "Filter")),
            (xne_TextureWrap_t)json_object_get_int(json_object_object_get(json_atlas, "Wrap"))
        );

        xne_link_texture_atlas(&sprite->textures, json_object_get_string(json_object_object_get(json_atlas, "Name")), sprite->shader.program);
    }
    else{
        fprintf(stdout, "cannot find texture atlas object!\n");
        free(fstr);
        json_object_put(__json_context);
        
        return XNE_FAILURE;
    }

    xne_create_transform(&sprite->transform);

    free(fstr);
    json_object_put(__json_context);

    return XNE_OK;
}

void xne_draw_sprite(xne_Sprite_t* sprite, xne_Camera_t* camera){
    xne_shader_enable(&sprite->shader);

    glBindTexture(GL_TEXTURE_2D_ARRAY, sprite->textures.target);
    xne_shader_use_uniform(&sprite->shader, 0, xne_get_camera_projection(camera));
    xne_shader_use_uniform(&sprite->shader, 1, xne_transform_lookat(&sprite->transform, camera->eye));
    xne_shader_use_uniform(&sprite->shader, 2, camera->eye);

    xne_draw_mesh(&sprite->plane);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    xne_shader_disable(NULL);
}

void xne_destroy_sprite(xne_Sprite_t* sprite){
    assert(sprite);

    xne_destroy_mesh(&sprite->plane);
    xne_destroy_shader(&sprite->shader);
    xne_destroy_atlas_texture(&sprite->textures);
    
    //memset(sprite, 0, sizeof(xne_Sprite_t));
}