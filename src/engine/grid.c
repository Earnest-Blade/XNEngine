#include "grid.h"

#include "graphics/camera.h"
#include "engine.h"

#include <assert.h>

int xne_create_grid(xne_Grid_t* grid, uint32_t width, uint32_t height, 
                    uint32_t tile_width, uint32_t tile_height){
    
    assert(grid);

    grid->width = width;
    grid->height = height;
    grid->tile_width = tile_width;
    grid->tile_height = tile_height;
    grid->color[0] = 1.0f;
    grid->color[1] = 1.0f;
    grid->color[2] = 1.0f;
    xne_create_plane(&grid->plane, width, height);

    const char* vert_shr = ""
        "#version 400\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec2 uvs;\n"
        "layout(location = 2) in vec3 normals;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 transform;\n"
        "out vec2 out_uv;\n"
        "void main(){\n"
        "   gl_Position = projection * transform * vec4(position, 1.0);\n"
        "   out_uv = uvs;\n"
        "}\n";
    
    const char* frag_shr = ""
        "#version 400\n"
        "in vec2 out_uv;\n"
        "uniform float tile_w;\n"
        "uniform float tile_h;\n"
        "uniform vec3 color;\n"
        "float grid(vec2 fragc, float space, float width){\n"
        "   vec2 p = fragc - vec2(0.5);\n"
        "   vec2 size = vec2(width);\n"
        "   vec2 a1 = mod(p - size, space);\n"
        "   vec2 a2 = mod(p + size, space);\n"
        "   vec2 a = abs(a1) - vec2(width/2);\n"
        "   return clamp(min(a.x, a.y), 0.0, 1.0);\n"
        "}"
        "void main(){\n"
        "   vec2 uv = out_uv * 2000;\n"
        "   vec3 col = color;\n"
        "   if(1 - grid(uv, 10.0f, 0.1f) > 0){gl_FragColor = vec4(col, 1.0);}\n"
        "   else {discard;}\n"
        "}\n";
    
    xne_create_shaderfv(&grid->shader, vert_shr, frag_shr);
    
    const xne_ShaderUniformDesc_t shader_uniforms[] = {
        {1, XNE_UNIFORM_MAT4, "projection"},
        {1, XNE_UNIFORM_MAT4, "transform"},
        {1, XNE_UNIFORM_FLOAT, "tile_w"},
        {1, XNE_UNIFORM_FLOAT, "tile_h"},
        {1, XNE_UNIFORM_VEC3, "color"},
        XNE_SHADER_UNIFORM_END()
    };

    xne_link_shader_uniforms(&grid->shader, shader_uniforms);

    xne_create_transform(&grid->transform);
}

void xne_draw_grid(xne_Grid_t* grid){
    xne_shader_enable(&grid->shader);
    
    xne_shader_use_uniform(&grid->shader, 0, xne_get_camera_projection(&xne_get_engine_instance()->state.scene->camera));
    xne_shader_use_uniform(&grid->shader, 1, xne_transform_matrix(&grid->transform));
    xne_shader_use_uniform(&grid->shader, 2, &grid->tile_width);
    xne_shader_use_uniform(&grid->shader, 3, &grid->tile_height);
    xne_shader_use_uniform(&grid->shader, 4, &grid->color[0]);

    xne_draw_mesh(&grid->plane);

    xne_shader_disable(NULL);
}

void xne_destroy_grid(xne_Grid_t* grid){
    xne_destroy_shader(&grid->shader);
    xne_destroy_mesh(&grid->plane);
}