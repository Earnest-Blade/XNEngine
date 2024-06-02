#pragma once

#ifndef XNE_TRANSFORM_IMPL
#define XNE_TRANSFORM_IMPL

typedef struct xne_Transform {
    float position[3];
    float rotation[4];
    float scale[3];
    float local[4][4];
    float world[4][4];
} xne_Transform_t;

void xne_create_transform(xne_Transform_t* transform);

void xne_transform_moveat(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_moveto(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_scaleat(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_scaleto(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_rotateat(xne_Transform_t* transform, float roll, float pitch, float yaw);
void xne_transform_rotateto(xne_Transform_t* transform, float roll, float pitch, float yaw);

void xne_transform_direction_forward(xne_Transform_t* transform, float dest[3]);
void xne_transform_direction_up(xne_Transform_t* transform, float dest[3]);
void xne_transform_direction_left(xne_Transform_t* transform, float dest[3]);
/*
    Extract transform direction such as:
    
     forward.x  | forward.y | forward.z
     up.x       | up.y      | up.z
     left.x     | left.y    | left.z 
*/
static inline void xne_transform_directions(xne_Transform_t* transform, float dest[3][3]){
    xne_transform_direction_forward(transform, dest[0]);
    xne_transform_direction_up(transform, dest[1]);
    xne_transform_direction_left(transform, dest[2]);
}

inline static float* xne_transform_matrix(xne_Transform_t* transform) { return &transform->local[0][0]; }
float* xne_transform_lookat(xne_Transform_t* transform, float eye[3]);


#endif