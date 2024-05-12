#pragma once

#ifndef XNE_UI_IMPL
#define XNE_UI_IMPL

#include "core/core.h"

typedef enum xne_UiAntiAliasing {
    XNE_ANTI_ALIASING_DISABLE,
    XNE_ANTI_ALIASING_ENABLE
} xne_UiAntiAliasing_t;

typedef struct xne_UserDevice {
    struct nk_context* ctx;
    struct nk_font_atlas* atlas;
    void* internal;
    size_t bufsize;

    xne_UiAntiAliasing_t AA;
} xne_UserDevice_t;

typedef struct xne_UserDeviceDesc {
    void* device;
    void* graphics;
    uint32_t bufsize;
    xne_UiAntiAliasing_t AA;
} xne_UserDeviceDesc_t;

void xne_create_user_device(xne_UserDevice_t* device, xne_UserDeviceDesc_t desc);
void xne_user_device_new_frame(xne_UserDevice_t* device);
void xne_user_device_draw(xne_UserDevice_t* device);
void xne_destroy_user_device(xne_UserDevice_t* device);

#endif
