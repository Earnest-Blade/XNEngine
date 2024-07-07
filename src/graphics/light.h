#pragma once

#ifndef XNE_LIGHT_IMPL
#define XNE_LIGHT_IMPL

#include "core/core.h"
#include "core/math.h"

typedef enum xne_LightType {
    XNE_LIGHT_NONE = 0,
    XNE_LIGHT_DIRECTIONAL = 1,
    XNE_LIGHT_POINT = 2,
    XNE_LIGHT_SPOT = 3
} xne_LightType_t;

typedef struct xne_DirectionalLight {
    xne_vec3 direction;
    xne_Colorf_t color;
} xne_DirectionalLight_t;

typedef struct xne_PointLight {
    xne_Colorf_t color;
    float constant, linear, quadratic;
} xne_PointLight_t;

typedef struct xne_SpotLight {
    xne_vec3 direction;
    xne_Colorf_t color;
    float constant, linear, quadratic;
    float cut_off, outer_cut_off;
} xne_SpotLight_t;

typedef struct xne_Light {
    xne_LightType_t type;
    union {
        xne_DirectionalLight_t directional;
        xne_PointLight_t point;
        xne_SpotLight_t spot;
    } light;

    xne_vec3 position;
} xne_Light_t;

void xne_create_light(xne_Light_t* light, xne_LightType_t type);

#endif