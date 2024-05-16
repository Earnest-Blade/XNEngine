#include "camera.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <cglm/cglm.h>
#include <cglm/quat.h>

static void xne__process_projection(xne_Camera_t* camera){
    if(camera->mode == XNE_CAMERA_PERSPECTIVE){
        const float aspect = (float)*camera->width / (float)*camera->height;
        const float fn = tan(glm_rad(camera->fov) / 2);

        memset(camera->screen, 0, sizeof(camera->screen));
        camera->screen[0][0] = 1 / (aspect * fn);
        camera->screen[1][1] = 1 / fn;
        camera->screen[2][2] = (-(camera->far + camera->near) / (camera->far - camera->near));
        camera->screen[3][2] = (-(2 * camera->far * camera->near) / (camera->far - camera->near));
        camera->screen[2][3] = -1;
        
        return;
    }
    assert(0);
}

static void xne__process_view(xne_Camera_t* camera){
    float front[3], right[3], up[3];
    float upv[3] = {0.0f, 1.0f, 0.0f};

    glm_normalize_to(camera->direction, front);

    glm_vec3_crossn(front, upv, right);
    glm_vec3_crossn(right, front, up);

    glm_vec3_add(camera->eye, front, front);

    glm_vec3_sub(front, camera->eye, front);
    glm_vec3_normalize_to(front, front);

    float side[3];
    glm_vec3_crossn(front, up, side);
    glm_vec3_crossn(side, front, up);

    camera->view[0][0] = side[0];
    camera->view[1][0] = side[1];
    camera->view[2][0] = side[2];
    camera->view[3][0] = -glm_vec3_dot(side, camera->eye);
    camera->view[0][1] = up[0];
    camera->view[1][1] = up[1];
    camera->view[2][1] = up[2];
    camera->view[3][1] = -glm_vec3_dot(up, camera->eye);
    camera->view[0][2] = -front[0];
    camera->view[1][2] = -front[1];
    camera->view[2][2] = -front[2];
    camera->view[3][2] = glm_vec3_dot(front, camera->eye);
    camera->view[0][3] = 0.0f;
    camera->view[1][3] = 0.0f;
    camera->view[1][3] = 0.0f;
    camera->view[3][3] = 1.0f;
}

void xne_create_camera(xne_Camera_t* camera, xne_Camera_Desc_t desc){
    assert(camera);
    camera->mode = desc.projection;
    camera->width = desc.width;
    camera->height = desc.height;
    camera->fov = desc.fov;
    camera->near = desc.near;
    camera->far = desc.far;

    memset(camera->eye, 0, sizeof(camera->eye));
    memset(camera->direction, 0, sizeof(camera->direction));

    xne__process_projection(camera);
    xne__process_view(camera);
}

void xne_camera_moveat(xne_Camera_t* camera, float x, float y, float z){
    camera->eye[0] = x;
    camera->eye[1] = y;
    camera->eye[2] = z;
}

void xne_camera_moveto(xne_Camera_t* camera, float x, float y, float z){
    camera->eye[0] += x;
    camera->eye[1] += y;
    camera->eye[2] += z;
}

void xne_camera_rotateat(xne_Camera_t* camera, float roll, float yaw, float pitch){
#ifdef XNE_DISABLE_PITCH_FLIP
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
#endif

    camera->look[0] = roll;
    camera->look[1] = yaw;
    camera->look[2] = pitch;

    camera->direction[0] = cos(glm_rad(camera->look[1])) * cos(glm_rad(camera->look[2]));
    camera->direction[1] = sin(glm_rad(camera->look[2]));
    camera->direction[2] = sin(glm_rad(camera->look[1])) * cos(glm_rad(camera->look[2]));
}

void xne_camera_rotateto(xne_Camera_t* camera, float roll, float yaw, float pitch){
#ifdef XNE_DISABLE_PITCH_FLIP
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
#endif
    
    camera->look[0] += roll;
    camera->look[1] += yaw;
    camera->look[2] += pitch;

    camera->direction[0] = cos(glm_rad(camera->look[1])) * cos(glm_rad(camera->look[2]));
    camera->direction[1] = sin(glm_rad(camera->look[2]));
    camera->direction[2] = sin(glm_rad(camera->look[1])) * cos(glm_rad(camera->look[2]));
}

float* xne_get_camera_projection(xne_Camera_t* camera){
    xne__process_projection(camera);
    xne__process_view(camera);

    glm_mat4_mul(camera->screen, camera->view, camera->projection);
    return &camera->projection[0][0];
}