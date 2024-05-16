#include "sprite.h"

#include <gl/glew.h>
#include <cglm/cglm.h>

#include "objects.h"

#include <memory.h>
#include <assert.h>

int xne_create_sprited(xne_Sprite_t* sprite, xne_SpriteDesc_t desc){
    assert(sprite);
    memset(sprite, 0, sizeof(xne_Sprite_t));

    sprite->frame = 0;
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

    xne_MeshDesc_t mdesc;
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

    xne_create_texture_atlas(&sprite->atlas, desc.path, desc.tile_width_count, 
        desc.tile_height_count, desc.filter, desc.wrap);
    xne_link_texture_atlas(&sprite->atlas, "texture0", sprite->shader.program);
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

    if(json_object_get_type(json_sprite) != json_type_null){
        sprite->frame = 0;
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
        
        xne_MeshDesc_t mesh_desc;
        mesh_desc.vertices = vertices;
        mesh_desc.elements = elements;
        mesh_desc.vertex_align = align;
        mesh_desc.vertices_count = 4;
        mesh_desc.elements_count = 6;
        mesh_desc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;

        xne_create_mesh(&sprite->plane, mesh_desc);

        xne_link_texture_atlas_layer(&sprite->atlas, json_object_get_string(json_object_object_get(json_sprite, "Layer")), sprite->shader.program);
    }
    else{
        fprintf(stdout, "cannot find sprite object!\n");
        free(fstr);
        json_object_put(__json_context);

        return XNE_FAILURE;
    }

    if(json_object_get_type(json_atlas) != json_type_null){
        xne_create_texture_atlas(
            &sprite->atlas,
            json_object_get_string(json_object_object_get(json_atlas, "Path")),
            json_object_get_int(json_object_object_get(json_atlas, "TileWidthCount")),
            json_object_get_int(json_object_object_get(json_atlas, "TileHeightCount")),
            (xne_TextureFilter_t)json_object_get_int(json_object_object_get(json_atlas, "Filter")),
            (xne_TextureWrap_t)json_object_get_int(json_object_object_get(json_atlas, "Wrap"))
        );

        xne_link_texture_atlas(&sprite->atlas, json_object_get_string(json_object_object_get(json_atlas, "Name")), sprite->shader.program);
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

    glBindTexture(GL_TEXTURE_2D_ARRAY, sprite->atlas.target);
    //glUniform1i(sprite->textures.texture_location, XNE_TEXTURE_UNIT0);

#ifndef XNE_SPRITE_NO_ROTATE

    const float steps = 22.5f;
    xne_SpriteDirection_t direction = XNE_SPRITE_FORWARD;

    {
        float plane[3], up[3], forward[3], cross[3];

        xne_transform_direction_up(&sprite->transform, up);
        xne_transform_direction_forward(&sprite->transform, forward);
        //glm_vec3_copy(sprite->transform.position, forward);

        // Project On Plane
        float mag = glm_vec3_dot(up, up);
        float dot_plane = glm_vec3_dot(camera->eye, up);
        plane[0] = camera->direction[0] - up[0] * dot_plane / mag;
        plane[1] = camera->direction[1] - up[1] * dot_plane / mag;
        plane[2] = camera->direction[2] - up[2] * dot_plane / mag;

        // Angle
        float angledet = sqrtf(sqrt(glm_vec3_dot(plane, plane)) * sqrt(glm_vec3_dot(forward, forward))); 
        float angledot = glm_clamp(glm_vec3_dot(plane, forward) / angledet, -1.0, 1.0);
        float anglecos = acosf(angledot) * M_2_PI;

        // Signed Angle
        glm_vec3_cross(plane, forward, cross);
        float sign = glm_signf(up[0] * cross[0] + up[1] * cross[1] + up[2] * cross[2]);
        float angle = sign * anglecos * 100;
        
        const float absangle = abs(angle);
        if(absangle < steps) direction = XNE_SPRITE_FORWARD;
        if(absangle < steps * 3) direction = XNE_SPRITE_RIGHT;
        if(absangle < steps * 5) direction = XNE_SPRITE_BACKWARD;
        if(absangle < steps * 7) direction = XNE_SPRITE_LEFT;

        sprite->frame = sprite->frame + direction;
    }

#endif

    glUniform1i(sprite->atlas.layer_location, sprite->frame);

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
    xne_destroy_atlas_texture(&sprite->atlas);
    
    //memset(sprite, 0, sizeof(xne_Sprite_t));
}