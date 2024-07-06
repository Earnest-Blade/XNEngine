#pragma once

#ifndef XNE_MATH_IMPL
#define XNE_MATH_IMPL

#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>

#define XNE_MATH_TYPES

#ifdef XNE_MATH_TYPES

typedef float xne_vec2[2];
typedef float xne_vec3[3];
typedef float xne_vec4[4];

typedef int xne_vec2i[2];
typedef int xne_vec3i[3];
typedef int xne_vec4i[4];

typedef xne_vec3 xne_mat3[3];
typedef xne_vec4 xne_mat4[4];

#endif

#define XNE_CLAMP(val, low, high) (val < low) ? low : ((val > high) ? high : val)

#define XNE_IDENTITY_MATRIX3(mat){              \
        memset(mat, 0, sizeof(xne_mat3));       \
        mat[2][2] = 1.0f;                       \
    }


#define XNE_IDENTITY_MATRIX4(mat){              \
        memset(mat, 0, sizeof(xne_mat4));       \
        mat[3][3] = 1.0f;                       \
    }

static inline float xne_to_rad(float deg) { 
    return deg * M_PI / 180.0f; 
}

static inline float xne_to_deg(float rad){
    return rad * 180.0f / M_PI;
}

static inline void xne_perpective_projection(float aspect, float fov, float near, float far, xne_mat4 dest){
    XNE_IDENTITY_MATRIX4(dest);

    const float ysc = 1.0f / tan(xne_to_rad(fov) * 0.5f);
    const float fn = 1.0f / (far - near);

    dest[0][0] = ysc * (1.0f / aspect);
    dest[1][1] = ysc;
    dest[2][2] = -(far + near) * fn;
    dest[3][2] = -(2.0f * far * near) * fn;
    dest[2][3] = -1.0f;
}

static void xne_orthographic_projection(float width, float height, float near, float far, float scale, xne_mat4 dest){
    XNE_IDENTITY_MATRIX4(dest);
    
    const float lr = 1.0f / (width);
    const float bt = 1.0f / (height);
    const float fn = 1.0f / (far - near);

    dest[0][0] = 2.0f * lr * scale;
    dest[1][1] = 2.0f * bt * scale;
    dest[2][2] = fn;
    dest[0][3] = -(-(width * 0.5f) + (width * 0.5f)) * lr;
    dest[1][3] = -((height * 0.5f) -(height * 0.5f)) * fn;
    dest[2][3] = -near * fn;
    dest[3][3] = 1.0f;
}

static void xne_orthographic_projection_off_center(float left, float right, float bottom, float top, float far, float near, float scale, xne_mat4 dest){
    XNE_IDENTITY_MATRIX4(dest);
    left /= scale;
    right /= scale;
    top /= scale; 
    bottom /= scale; 
    
    float rl = 1.0f / (right - left);
    float tb = 1.0f / (top - bottom);
    float fn = -1.0f / (far - near);

    dest[0][0] = 2.0f * rl;
    dest[1][1] = 2.0f * tb;
    dest[2][2] = 2.0f * fn;
    dest[3][0] =-(right + left) * rl;
    dest[3][1] =-(top + bottom) * tb;
    dest[3][2] = (far + near) * fn;
    dest[3][3] = 1.0f;
}

#endif