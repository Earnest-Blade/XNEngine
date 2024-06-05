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
    int* width; 
    int *height;
    float fov;
    float near;
    float far;
} xne_Camera_Desc_t;

typedef struct xne_Camera {
    xne_Camera_Projection_t mode;
    int* width;
    int* height;
    float fov, near, far;
    xne_vec3 eye;
    xne_vec3 look;
    xne_vec3 direction;

    xne_mat4 screen;
    xne_mat4 view;
    xne_mat4 projection;
} xne_Camera_t;

void xne_create_camera(xne_Camera_t* camera, xne_Camera_Desc_t desc);
void xne_camera_moveat(xne_Camera_t* camera, float x, float y, float z);
void xne_camera_moveto(xne_Camera_t* camera, float x, float y, float z);
void xne_camera_rotateat(xne_Camera_t* camera, float roll, float yaw, float pitch);
void xne_camera_rotateto(xne_Camera_t* camera, float roll, float yaw, float pitch);
float* xne_get_camera_projection(xne_Camera_t* camera);

#endif
