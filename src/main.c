
#define XNE_CORE_BUFFER
#include "core/core.h"

#define XNE_INLUDE_MESH_GEN
#define XNE_IMPLEMENTATION
#include "xne.h"

#include <stdio.h>

static struct xne_Game {
    xne_Shader_t shader;
    xne_Texture_t texture;

    xne_Scene_t scene;

    xne_Sprite_t sprite;
    xne_Model_t model;
} state;

static inline void xne__update_free_camera(xne_Camera_t* camera, float delta, float speed, float sensivity){
    float position[3] = {0.0f, 0.0f, 0.0f};

    if(xne_key_down(XNE_KEY_LEFT)) position[0] = -speed * delta;
    if(xne_key_down(XNE_KEY_RIGHT)) position[0] = speed * delta;
    if(xne_key_down(XNE_KEY_UP)) position[2] = -speed * delta;
    if(xne_key_down(XNE_KEY_DOWN)) position[2] = speed * delta;

    xne_camera_moveto(camera, position[0], position[1], position[2]);

    if(xne_mouse_button_down(XNE_MOUSE_BUTTON_3)){
        float x, y;
        xne_get_mouse_delta_position(&x, &y);
        x *= delta * sensivity;
        y *= delta * sensivity;

        if(x || y){
            xne_camera_rotateto(camera, 0, x, y);
        }
    }
}

int main(int argc, char** argv){
    xne_create_engine_instance();

    xne_Device_t window;
    xne_create_device(&window, 1920, 1080, "XNEngine", XNE_WINDOW_DEFAULT);
    
    xne_GraphicDeviceDesc_t graphic_device_desc;
    graphic_device_desc.device = &window;
    graphic_device_desc.bufferWidth = window.framebufferSize[0];
    graphic_device_desc.bufferHeight = window.framebufferSize[1];
    graphic_device_desc.clearColor[0] = 0.0f;
    graphic_device_desc.clearColor[1] = 0.0f;
    graphic_device_desc.clearColor[2] = 0.0f;
    
    xne_GraphicDevice_t graphics;
    xne_create_graphic_device(&graphics, graphic_device_desc);

    xne_Camera_Desc_t camera_desc;
    camera_desc.projection = XNE_CAMERA_PERSPECTIVE;
    camera_desc.width = (int*) &window.framebufferSize[0];
    camera_desc.height = (int*) &window.framebufferSize[1];
    camera_desc.fov = 60.0f;
    camera_desc.far = 100.0f;
    camera_desc.near = 0.1f;

    xne_create_scene(&state.scene, "Hello scene", &camera_desc);
    xne_get_engine_instance()->state.scene = &state.scene;

    xne_ShaderDesc_t shaders[3] = {
        {XNE_VERTEX_SHADER, "_VERTEX_"},
        {XNE_FRAGMENT_SHADER, "_FRAGMENT_"},
        {0, ""}
    };

    xne_ShaderUniformDesc_t uniforms[3] = {
        {XNE_UNIFORM_ATTRIB_UNIFORM, XNE_UNIFORM_MAT4, "projection"},
        {XNE_UNIFORM_ATTRIB_UNIFORM, XNE_UNIFORM_MAT4, "transform"},
        {XNE_SHADER_UNIFORM_END()}
    };

    xne_create_model(&state.model, "assets/objects/cube.xneo");

    /*xne_SpriteDesc_t sprite;
    sprite.path = "assets/images/one_spritesheet.png";
    sprite.shader = "shaders/sprite.glsl";
    sprite.sprite_width = 1.0f;
    sprite.sprite_height = 1.0f;
    sprite.tile_width_count = 4;
    sprite.tile_height_count = 4;
    sprite.shader_desc = shaders;
    sprite.uniform_desc = uniforms;
    sprite.wrap = XNE_TEXTURE_WRAP_REPEAT;
    sprite.filter = XNE_TEXTURE_FILTER_NEAREST;*/

    xne_create_sprite(&state.sprite, "assets/objects/one_spritesheet.xneo");
    state.sprite.frame = 1;

    xne_camera_rotateat(&state.scene.camera, 0, -90.0f, 0);
    xne_camera_moveat(&state.scene.camera, 0, 0, 5);

    while (!xne_is_done())
    {
        xne_get_engine_instance()->state.delta_time = xne_get_time() - xne_get_engine_instance()->state.late_time;
        xne_get_engine_instance()->state.late_time = xne_get_time();

        xne_poll_events(&window);
        xne_new_frame(&graphics);

        if(xne_key_down(XNE_KEY_ESCAPE)) xne_done();
        xne__update_free_camera(
            &state.scene.camera,
            xne_get_engine_instance()->state.delta_time,
            2.0f, 200.0f 
        );

        if(xne_key_down(XNE_KEY_A)) state.sprite.frame = 1;
        if(xne_key_down(XNE_KEY_E)) state.sprite.frame = 6;

        xne_draw_sprite(&state.sprite, &xne_get_engine_instance()->state.scene->camera);
        xne_draw_model(&state.model);

/*
        xne_shader_enable(&state.shader);
        xne_texture_enable(&state.texture, XNE_TEXTURE_UNIT0);

        xne_shader_use_uniform(&state.shader, 0, xne_get_camera_projection(&state.scene.camera));
        xne_draw_mesh(&state.mesh);

        xne_texture_disable(NULL);
        xne_shader_disable(NULL);
*/

        xne_blit_buffers(&graphics);
        xne_swap_buffers(&window);
    }

/*    
    xne_destroy_shader(&state.shader);
    xne_destroy_mesh(&state.mesh); */
    xne_destroy_model(&state.model);
    xne_destroy_sprite(&state.sprite);

    
    xne_destroy_graphic_device(&graphics);
    xne_destroy_device(&window);

    xne_destroy_engine_instance();

    return 0;
}