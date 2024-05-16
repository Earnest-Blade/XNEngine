#pragma once

#ifndef XNE_CAMERA_IMPL
#define XNE_CAMERA_IMPL

#include "core/core.h"

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
    float eye[3];
    float look[3];
    float direction[3];

    float screen[4][4];
    float view[4][4];
    float projection[4][4];
} xne_Camera_t;

void xne_create_camera(xne_Camera_t* camera, xne_Camera_Desc_t desc);
void xne_camera_moveat(xne_Camera_t* camera, float x, float y, float z);
void xne_camera_moveto(xne_Camera_t* camera, float x, float y, float z);
void xne_camera_rotateat(xne_Camera_t* camera, float roll, float yaw, float pitch);
void xne_camera_rotateto(xne_Camera_t* camera, float roll, float yaw, float pitch);
float* xne_get_camera_projection(xne_Camera_t* camera);

#endif
