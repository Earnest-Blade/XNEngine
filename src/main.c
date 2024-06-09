#define XNE_CORE_BUFFER
#include "core/core.h"

#define XNE_INLUDE_MESH_GEN
#define XNE_IMPLEMENTATION
#include "xne.h"

#include <stdio.h>

#define XNE_CAMERA_DISTANCE 3

static struct md_GameState {
    xne_Shader_t shader;
    xne_Texture_t texture;

    xne_Scene_t scene;

    xne_Sprite_t sprite;
    xne_Model_t model;
    xne_Mesh_t floor;
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

static inline void xne__update_player_mvt(xne_Camera_t* camera, float delta, float speed, float sensivity){
    float position[3] = {0.0f, 0.0f, 0.0f};

    if(xne_key_down(XNE_KEY_LEFT)) position[0] = -speed * delta;
    if(xne_key_down(XNE_KEY_RIGHT)) position[0] = speed * delta;
    if(xne_key_down(XNE_KEY_UP)) position[2] = speed * delta;
    if(xne_key_down(XNE_KEY_DOWN)) position[2] = -speed * delta;

    xne_camera_moveto(camera, position[0], position[1], position[2]);
    xne_transform_move_to(&state.sprite.transform, position[0], position[1], position[2]);
}

static void xne__initialize(xne_Device_t* window, xne_GraphicDevice_t* graphics){
    xne_create_device(window, 1920, 1080, "XNEngine", XNE_WINDOW_DEFAULT);
    
    xne_GraphicDeviceDesc_t graphic_device_desc;
    graphic_device_desc.device = window;
    graphic_device_desc.buffer_width = window->framebufferSize[0];
    graphic_device_desc.buffer_height = window->framebufferSize[1];
    graphic_device_desc.framebuffer_shader = "shaders/framebuffer.glsl";
    xne_set_color_rgb(graphic_device_desc.clear_color, 0, 0, 0);
    
    xne_create_graphic_device(graphics, graphic_device_desc);

    xne_Camera_Desc_t camera_desc;
    camera_desc.projection = XNE_CAMERA_ORTHO;
    camera_desc.width = (int*) &window->framebufferSize[0];
    camera_desc.height = (int*) &window->framebufferSize[1];
    camera_desc.fov = 60.0f;
    camera_desc.far = 100.0f;
    camera_desc.near = 0.0f;

    xne_create_scene(&state.scene, "Hello scene", &camera_desc);

    xne_create_model(&state.model, "assets/objects/floor.xneo");

    xne_create_sprite(&state.sprite, "assets/objects/one_spritesheet.xneo");
    state.sprite.frame = 1;

    xne_camera_rotate(&state.scene.camera, 0.0f, -90.0f, 30.0f);
    xne_camera_move(&state.scene.camera, 0, 0.5f, XNE_CAMERA_DISTANCE);
}

static void xne__destroy(xne_Device_t* window, xne_GraphicDevice_t* graphics){
    xne_destroy_model(&state.model);
    xne_destroy_sprite(&state.sprite);
    xne_destroy_scene(&state.scene);
    
    xne_destroy_graphic_device(graphics);
    xne_destroy_device(window);
    memset(&state, 0, sizeof(struct md_GameState));
}

int main(int argc, char** argv){
    xne_create_engine_instance();

    xne_Device_t window;
    xne_GraphicDevice_t graphics;

    xne__initialize(&window, &graphics);

    while (!xne_is_done())
    {
        xne_get_engine_instance()->state.delta_time = xne_get_time() - xne_get_engine_instance()->state.late_time;
        xne_get_engine_instance()->state.late_time = xne_get_time();

        xne_poll_events(&window);
        xne_new_frame(&graphics);

        if(xne_key_down(XNE_KEY_ESCAPE)) xne_done();

        if(xne_key_down(XNE_KEY_R)){
            xne__destroy(&window, &graphics);
            xne__initialize(&window, &graphics);
        }

        xne__update_player_mvt(
            &state.scene.camera,
            xne_get_engine_instance()->state.delta_time,
            2.0f, 200.0f 
        );

        xne_draw_billboard_sprite(&state.sprite, &xne_get_engine_instance()->state.scene->camera);
        xne_draw_model(&state.model);

        xne_blit_buffers(&graphics);
        xne_swap_buffers(&window);
    }

    
    xne__destroy(&window, &graphics);
    xne_destroy_engine_instance();

    return XNE_OK;
}