#pragma once

#ifndef XNE_CAMERA_IMPL
#define XNE_CAMERA_IMPL

#include "core/core.h"
#include "core/math.h"

#define XNE_DISABLE_PITCH_FLIP

typedef enum xne_Camera_Projection {
    XNE_CAMERA_PERSPECTIVE,
    XNE_CAMERA_ORTHO
} xne_Camera_Projection_t;

typedef struct xne_Camera_Desc {
    xne_Camera_Projection_t projection;
    int width; 
    int height;
    float fov;
    float near;
    float far;
} xne_Camera_Desc_t;

typedef struct xne_Camera {
    xne_Camera_Projection_t mode;
    int width;
    int height;
    float fov, near, far;
    xne_vec3 eye;
    xne_vec3 look;
    xne_vec3 direction;

    xne_mat4 screen;
    xne_mat4 view;
    xne_mat4 projection;
} xne_Camera_t;

void xne_create_camera(xne_Camera_t* camera, xne_Camera_Desc_t* desc);

void xne_camera_move(xne_Camera_t* camera, float x, float y, float z);
static inline void xne_camera_movevf(xne_Camera_t* camera, float* values){
    xne_camera_move(camera, values[0], values[1], values[2]);
}

void xne_camera_move_to(xne_Camera_t* camera, float x, float y, float z);
static inline void xne_camera_move_tovf(xne_Camera_t* camera, float* values){
    xne_camera_move_to(camera, values[0], values[1], values[2]);
}

void xne_camera_rotate(xne_Camera_t* camera, float roll, float yaw, float pitch);
static inline void xne_camera_rotatevf(xne_Camera_t* camera, float* values){
    xne_camera_rotate(camera, values[0], values[1], values[2]);
}

void xne_camera_rotate_to(xne_Camera_t* camera, float roll, float yaw, float pitch);
static inline void xne_camera_rotate_tovf(xne_Camera_t* camera, float* values){
    xne_camera_rotate_to(camera, values[0], values[1], values[2]);
}

float* xne_get_camera_projection(xne_Camera_t* camera);

#endif
