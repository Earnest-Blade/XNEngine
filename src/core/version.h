#pragma once

#ifndef XNE_VERSION_IMPL
#define XNE_VERSION_IMPL

#include <stdint.h>

typedef struct xne_Version {
    uint16_t major, minor, patch;
} xne_Version_t;

#endif