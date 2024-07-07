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

/*
    Create a new transform.
    @param transform Pointer to the transform to initialize.
*/
void xne_create_transform(xne_Transform_t* transform);

/*
    Add an other transform to another.
    @param dest First transform.
    @param other Second transform.
*/
void xne_transform_add(xne_Transform_t* dest, const xne_Transform_t* other);


/*
    Substract an other transform to another.
    @param dest First transform.
    @param other Second transform.
*/
void xne_transform_sub(xne_Transform_t* dest, const xne_Transform_t* other);

/*
    Define new transform's position.
    @param transform Pointer to the target transform.
    @param x X position value.
    @param y Y position value.
    @param z Z position value.
*/
void xne_transform_move(xne_Transform_t* transform, float x, float y, float z);

/*
    Define new transform's position.
    @param transform Pointer to the target transform.
    @param position New x, y, z values.
*/
static inline void xne_transform_movevf(xne_Transform_t* transform, float position[3]){
    xne_transform_move(transform, position[0], position[1], position[2]);
}

/*
    Add values to transform's position.
    @param transform Pointer to the target transform.
    @param x X position value.
    @param y Y position value.
    @param z Z position value.
*/
void xne_transform_move_to(xne_Transform_t* transform, float x, float y, float z);

/*
    Add values to transform's position.
    @param transform Pointer to the target transform.
    @param position x, y, z values.
*/
static inline void xne_transform_move_tovf(xne_Transform_t* transform, float position[3]){
    xne_transform_move_to(transform, position[0], position[1], position[2]);
}

/*
    Define new transform's scale.
    @param transform Pointer to the target transform.
    @param x X scale value.
    @param y Y scale value.
    @param z Z scale value.
*/
void xne_transform_scale(xne_Transform_t* transform, float x, float y, float z);

/*
    Define new transform's scale.
    @param transform Pointer to the target transform.
    @param v x, y, z value
*/
static inline void xne_transform_scalef(xne_Transform_t* transform, float v){
    xne_transform_scale(transform, v, v, v);
}

/*
    Define new transform's scale.
    @param transform Pointer to the target transform.
    @param v x, y, z values
*/
static inline void xne_transform_scalefv(xne_Transform_t* transform, float values[3]){
    xne_transform_scale(transform, values[0], values[1], values[2]);
}

/*
    Add values to transform's scale.
    @param transform Pointer to the target transform.
    @param x X scale value.
    @param y Y scale value.
    @param z Z scale value.
*/
void xne_transform_scale_to(xne_Transform_t* transform, float x, float y, float z);

/*
    Add values to transform's scale.
    @param transform Pointer to the target transform.
    @param x x, y, z scale value.
*/
static inline void xne_transform_scale_tof(xne_Transform_t* transform, float v){
    xne_transform_scale_to(transform, v, v, v);
}

/*
    Add values to transform's scale.
    @param transform Pointer to the target transform.
    @param x x, y, z scale values.
*/
static inline void xne_transform_scale_tofv(xne_Transform_t* transform, float values[3]){
    xne_transform_scale_to(transform, values[0], values[1], values[2]);
}

/*
    Define new transform's rotation using euler angles.
    @param transform Pointer to the target transform.
    @param roll X angle value.
    @param pitch Y angle value.
    @param yaw Z angle value.
*/
void xne_transform_rotate(xne_Transform_t* transform, float roll, float pitch, float yaw);

/*
    Define new transform's rotation using euler angles.
    @param transform Pointer to the target transform.
    @param values Roll, Pitch and Yaw values.
*/
static inline void xne_transform_rotatefv(xne_Transform_t* transform, float values[3]){
    xne_transform_rotate(transform, values[0], values[1], values[2]);
}

/*
    Add values to transform's rotation using euler angles.
    @param transform Pointer to the target transform.
    @param roll X angle value.
    @param pitch Y angle value.
    @param yaw Z angle value.
*/
void xne_transform_rotate_to(xne_Transform_t* transform, float roll, float pitch, float yaw);

/*
    Add values to transform's rotation using euler angles.
    @param transform Pointer to the target transform.
    @param values Roll, Pitch and Yaw values.
*/
static inline void xne_transform_rotate_tofv(xne_Transform_t* transform, float values[3]){
    xne_transform_rotate_to(transform, values[0], values[1], values[2]);
}

void xne_transform_direction_forward(xne_Transform_t* transform, float dest[3]);
void xne_transform_direction_up(xne_Transform_t* transform, float dest[3]);
void xne_transform_direction_left(xne_Transform_t* transform, float dest[3]);

/*
    Extract transform direction such as:
    
     forward.x  | forward.y | forward.z
     up.x       | up.y      | up.z
     left.x     | left.y    | left.z 
    
    @param transform Pointer to the target transform.
    @param dest Destination matrix.
*/
static inline void xne_transform_directions(xne_Transform_t* transform, float dest[3][3]){
    xne_transform_direction_forward(transform, dest[0]);
    xne_transform_direction_up(transform, dest[1]);
    xne_transform_direction_left(transform, dest[2]);
}

/*
    Check if a transform is at the root of a model tree.
    @param transform Pointer to the target transform.
    @return 1 if the transform is the root.
*/
static inline int xne_transform_is_root(xne_Transform_t* transform){
    return transform->parent == (xne_Transform_t*)0;
}

/*
    Return the local matrix from a transform.
    @param transform Pointer to the target transform.
    @return 4x4 local matrix.
*/
inline static float* xne_transform_matrix(xne_Transform_t* transform) { return &transform->local[0][0]; }


/*
    Return the world matrix from a transform.
    @param transform Pointer to the target transform.
    @return 4x4 world matrix.
*/
inline static float* xne_transform_world_matrix(xne_Transform_t* transform) { return &transform->world[0][0]; }

/*
    Create a new look at matrix from a transform.
    @param transform Pointer to the target transform.
    @param eye Position to look at.
    @return 4x4 look at matrix.
*/
float* xne_transform_lookat(xne_Transform_t* transform, float eye[3]);


#endif