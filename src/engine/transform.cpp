#include "transform.h"

#include <cglm/cglm.h>

#include <memory.h>

#define xne_max(a,b) ((a) > (b) ? (a) : (b))

const static float xne__left_vecf[3] = {1.0f, 0.0f, 0.0f};
const static float xne__up_vecf[3] = {0.0f, 1.0f, 0.0f};
const static float xne__forward_vecf[3] = {0.0f, 0.0f, 1.0f};

static void xne__upadate_transform_mat(xne_Transform_t* transform){
    glm_mat4_identity(transform->matrix);

    glm_scale(transform->matrix, transform->scale);
    glm_translate(transform->matrix, transform->position);
    glm_quat_rotate(transform->matrix, transform->rotation, transform->matrix);
}

void xne_create_transform(xne_Transform_t* transform){
    memset(transform, 0, sizeof(xne_Transform_t));
    transform->scale[0] = 1;
    transform->scale[1] = 1;
    transform->scale[2] = 1;

    glm_quat_identity(transform->rotation);
    glm_mat4_identity(transform->matrix);

    xne__upadate_transform_mat(transform);
}

void xne_transform_moveat(xne_Transform_t* transform, float x, float y, float z){
    transform->position[0] = x;
    transform->position[1] = y;
    transform->position[2] = z;

    xne__upadate_transform_mat(transform);
}

void xne_transform_moveto(xne_Transform_t* transform, float x, float y, float z){
    transform->position[0] += x;
    transform->position[1] += y;
    transform->position[2] += z;

    xne__upadate_transform_mat(transform);
}

void xne_transform_scaleat(xne_Transform_t* transform, float x, float y, float z){
    transform->scale[0] = x;
    transform->scale[1] = y;
    transform->scale[2] = z;

    xne__upadate_transform_mat(transform);
}

void xne_transform_scaleto(xne_Transform_t* transform, float x, float y, float z){
    transform->scale[0] += x;
    transform->scale[1] += y;
    transform->scale[2] += z;

    xne__upadate_transform_mat(transform);
}

void xne_transform_rotateat(xne_Transform_t* transform, float roll, float pitch, float yaw){
    glm_quat_identity(transform->rotation);

    float quat0[4], quat1[4], quat2[4];
    
    glm_quatv(quat0, roll, (float*)xne__left_vecf);
    glm_quatv(quat1, pitch, (float*)xne__up_vecf);
    glm_quatv(quat2, yaw, (float*)xne__forward_vecf);

    glm_quat_mul(quat0, quat1, transform->rotation);
    glm_quat_mul(transform->rotation, quat2, transform->rotation);

    xne__upadate_transform_mat(transform);
}

void xne_transform_rotateto(xne_Transform_t* transform, float roll, float pitch, float yaw){
    float xquat[3] = {1.0f, 0.0f, 0.0f};
    float yquat[3] = {1.0f, 0.0f, 0.0f};
    float zquat[3] = {1.0f, 0.0f, 0.0f};
    float quat0[4], quat1[4], quat2[4], quat3[4];
    
    glm_quatv(quat0, roll, xquat);
    glm_quatv(quat1, pitch, yquat);
    glm_quatv(quat2, yaw, zquat);

    glm_quat_mul(quat0, quat1, quat3);
    glm_quat_mul(quat3, quat2, quat3);
    glm_quat_add(transform->rotation, quat3, transform->rotation);

    xne__upadate_transform_mat(transform);
}

/*
https://www.gamedev.net/forums/topic/56471-extracting-direction-vectors-from-quaternion/

forward vector:
x = 2 * (x*z + w*y)
y = 2 * (y*z - w*x)
z = 1 - 2 * (x*x + y*y)

up vector
x = 2 * (x*y - w*z)
y = 1 - 2 * (x*x + z*z)
z = 2 * (y*z + w*x)

left vector
x = 1 - 2 * (y*y + z*z)
y = 2 * (x*y + w*z)
z = 2 * (x*z - w*y)

*/

float* xne_transform_directions(xne_Transform_t* transform, mat3 directions){
    memset(directions, 0, sizeof(float) * 3 * 3);

    directions[0][0] = 2 * (transform->rotation[0] * transform->rotation[2] + transform->rotation[3] * transform->rotation[1]);
    directions[0][1] = 2 * (transform->rotation[1] * transform->rotation[2] - transform->rotation[3] * transform->rotation[0]);
    directions[0][2] = 1 - 2 * (sqrt(transform->rotation[0] + sqrt(transform->rotation[1])));
    
    directions[1][0] = 2 * (transform->rotation[0] * transform->rotation[1] - transform->rotation[3] * transform->rotation[2]);
    directions[1][1] = 1 - 2 * (sqrt(transform->rotation[0]) + sqrt(transform->rotation[2]));
    directions[1][2] = 2 * (transform->rotation[1] * transform->rotation[2] + transform->rotation[3] * transform->rotation[0]);
    
    directions[2][0] = 1 - 2 * (sqrt(transform->rotation[1]) + sqrt(transform->rotation[2]));
    directions[2][1] = 2 * (transform->rotation[0] * transform->rotation[1] + transform->rotation[3] * transform->rotation[2]);
    directions[2][2] = 2 * (transform->rotation[0] * transform->rotation[2] - transform->rotation[3] * transform->rotation[1]);
}

float* xne_transform_lookat(xne_Transform_t* transform, float eye[3]){
    mat4 lkmat;
    float forward[3], side[3], up[3];

    glm_vec3_sub(transform->position, eye, forward);
    glm_vec3_normalize_to(forward, forward);

    glm_vec3_crossn(forward, (float*)xne__up_vecf, side);
    glm_vec3_cross(side, forward, up);

    glm_mat4_identity(transform->matrix);

    lkmat[0][0] = side[0];
    lkmat[1][0] = side[1];
    lkmat[2][0] = side[2];
    lkmat[3][0] = -glm_vec3_dot(side, transform->position);

    lkmat[0][1] = up[0];
    lkmat[1][1] = up[1];
    lkmat[2][1] = up[2];
    lkmat[3][1] = -glm_vec3_dot(up, transform->position);
    
    lkmat[0][2] = -forward[0];
    lkmat[1][2] = -forward[1];
    lkmat[2][2] = -forward[2];
    lkmat[3][2] = glm_vec3_dot(forward, transform->position);    

    lkmat[0][3] = 0.0f;
    lkmat[1][3] = 0.0f;
    lkmat[2][3] = 0.0f;
    lkmat[3][3] = 1.0f;

    glm_mat4_inv(lkmat, transform->matrix);

    return &transform->matrix[0][0];
}