#pragma once

#ifndef XNE_TRANSFORM_IMPL
#define XNE_TRANSFORM_IMPL

typedef struct xne_Transform {
    struct xne_Transform* parent;
    float position[3];
    float rotation[4];
    float scale[3];
    float local[4][4];
    float world[4][4];
} xne_Transform_t;

void xne_create_transform(xne_Transform_t* transform);

void xne_transform_add(xne_Transform_t* dest, const xne_Transform_t* other);

void xne_transform_move(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_move_to(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_scale(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_scale_to(xne_Transform_t* transform, float x, float y, float z);
void xne_transform_rotate(xne_Transform_t* transform, float roll, float pitch, float yaw);
void xne_transform_rotate_to(xne_Transform_t* transform, float roll, float pitch, float yaw);

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

static inline int xne_transform_is_root(xne_Transform_t* transform){
    return transform->parent == (xne_Transform_t*)0;
}

inline static float* xne_transform_matrix(xne_Transform_t* transform) { return &transform->local[0][0]; }
inline static float* xne_transform_world_matrix(xne_Transform_t* transform) { return &transform->world[0][0]; }
float* xne_transform_lookat(xne_Transform_t* transform, float eye[3]);


#endif