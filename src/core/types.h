#pragma once 

#ifndef XNE_TYPE_IMPL
#define XNE_TYPE_IMPL

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define XNE_OK 0
#define XNE_FAILURE 1

#ifndef XNE_GET_SIZE
#define XNE_GET_SIZE(x)                                                        \
    ({size_t SIZE; switch(x){                                                  \
        case XNE_BYTE: case XNE_UNSIGNED_BYTE: SIZE = sizeof(char); break;     \
        case XNE_SHORT: case XNE_UNSIGNED_SHORT: SIZE = sizeof(short); break;  \
        case XNE_FLOAT: SIZE = sizeof(float); break;                           \
        case XNE_INT: case XNE_UNSIGNED_INT: SIZE = sizeof(int); break;        \
        default: SIZE = 0; break;                                              \
    }; SIZE; })
#endif

typedef unsigned int xne_Enum_t;

typedef uint8_t xne_Color_t[3];

typedef enum xne_Types {
    XNE_BYTE = 0x1400,
    XNE_UNSIGNED_BYTE = 0x1401,
    XNE_SHORT = 0x1402,
    XNE_UNSIGNED_SHORT = 0x1403,
    XNE_FLOAT = 0x1406,
    XNE_INT = 0x1404,
    XNE_UNSIGNED_INT = 0x1405,
} xne_Types_t;

typedef struct xne_Version {
    uint16_t major, minor, patch;
} xne_Version_t;

static inline void xne_set_color_rgb(xne_Color_t color, char r, char g, char b){
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

#endif