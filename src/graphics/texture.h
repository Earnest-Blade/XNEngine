#pragma once

#ifndef XNE_TEXTURE_IMPL
#define XNE_TEXTURE_IMPL

#define XNE_CORE_BUFFER
#define XNE_CORE_FILE
#include "core/core.h"

#include <stdio.h>
#include <string.h>

typedef enum xne_TextureFilter {
    XNE_TEXTURE_FILTER_NEAREST = 0x2600,
    XNE_TEXTURE_FILTER_LINEAR = 0x2601
} xne_TextureFilter_t;

typedef enum xne_TextureWrap {
    XNE_TEXTURE_WRAP_REPEAT = 0x2901,
    XNE_TEXTURE_WRAP_CLAMP_TO_EDGE = 0x812F
} xne_TextureWrap_t;

typedef enum xne_TextureSrc {
    XNE_UNKNOW = 0,
    XNE_BMP = 1,
    XNE_PNG = 2
} xne_TextureSrc_t;

typedef enum xne_TextureFormat {
    XNE_RED = 0x1903,
    XNE_RG = 0x8227,
    XNE_RGB = 0x1907,
    XNE_BGR = 0x80E0,
    XNE_RGBA = 0x1908,
    XNE_BGRA = 0x80E1
} xne_TextureFormat_t;

typedef enum xne_TextureSizedFormat {
    XNE_RED8 = 0x8229,
    XNE_RG8 = 0x822B,
    XNE_RGB8 = 0x8051,
    XNE_BGR8 = 0x8051,
    XNE_RGBA8 = 0x8058,
    XNE_BGRA8 = 0x8058
} xne_TextureSizedFormat_t;

typedef enum xne_TextureUnit {
    XNE_TEXTURE_UNIT0 = 0x84C0,
    XNE_TEXTURE_UNIT1 = 0x84C1,
    XNE_TEXTURE_UNIT2 = 0x84C2,
    XNE_TEXTURE_UNIT3 = 0x84C3,
    XNE_TEXTURE_UNIT4 = 0x84C4,
    XNE_TEXTURE_UNIT5 = 0x84C5,
    XNE_TEXTURE_UNIT6 = 0x84C6,
    XNE_TEXTURE_UNIT7 = 0x84C7,
    XNE_TEXTURE_UNIT8 = 0x84C8,
    XNE_TEXTURE_UNIT9 = 0x84C9,
    XNE_TEXTURE_UNIT10 = 0x84CA,
    XNE_TEXTURE_UNIT11 = 0x84CB,
    XNE_TEXTURE_UNIT12 = 0x84CC,
    XNE_TEXTURE_UNIT13 = 0x84CD,
    XNE_TEXTURE_UNIT14 = 0x84CE,
    XNE_TEXTURE_UNIT15 = 0x84CF,
    XNE_TEXTURE_UNIT16 = 0x84D0,
    XNE_TEXTURE_UNIT17 = 0x84D1,
    XNE_TEXTURE_UNIT18 = 0x84D2,
    XNE_TEXTURE_UNIT19 = 0x84D3,
    XNE_TEXTURE_UNIT20 = 0x84D4,
    XNE_TEXTURE_UNIT21 = 0x84D5,
    XNE_TEXTURE_UNIT22 = 0x84D6,
    XNE_TEXTURE_UNIT23 = 0x84D7,
    XNE_TEXTURE_UNIT24 = 0x84D8,
    XNE_TEXTURE_UNIT25 = 0x84D9,
    XNE_TEXTURE_UNIT26 = 0x84DA,
    XNE_TEXTURE_UNIT27 = 0x84DB,
    XNE_TEXTURE_UNIT28 = 0x84DC,
    XNE_TEXTURE_UNIT29 = 0x84DD,
    XNE_TEXTURE_UNIT30 = 0x84DE,
    XNE_TEXTURE_UNIT31 = 0x84DF
} xne_TextureUnit_t;

typedef struct xne_Image {
    int width, height, depth;
    int alignment;
    size_t channels;
    xne_TextureFormat_t format;
    xne_TextureSrc_t type;
    uint8_t* pixels;
} xne_Image_t;

/*
    Initialize an image using a file.

    @param dest Pointer to the image to create.
    @param file File to load into the image.
    
    @return 0 if failed initializing and 1 if succed.
*/
int xne_create_image(xne_Image_t* dest, FILE* file);

/*
    Flip vertically a given image.
*/
void xne_flip_image_vertically(xne_Image_t* src);

/*
    Freeing an image.

    @param src image to free.
*/
void xne_destroy_image(xne_Image_t* src);


typedef struct xne_Texture {
    xne_Image_t image;

    uint32_t target;
    int texture_location;
    xne_Enum_t filter, wrap;
} xne_Texture_t;

int xne_create_texturef(xne_Texture_t* texture, FILE* file, xne_TextureFilter_t filter, xne_TextureWrap_t wrap);
static inline int xne_create_texture(xne_Texture_t* texture, const char* path, xne_TextureFilter_t filter, xne_TextureWrap_t wrap) {
    FILE* file = fopen(path, "rb");
    int s = xne_create_texturef(texture, file, filter, wrap);
    fclose(file);
    return s;
}

int xne_link_texture(xne_Texture_t* texture, const char* name, uint32_t program);
void xne_texture_enable(xne_Texture_t* texture, xne_TextureUnit_t unit);
void xne_texture_disable(xne_Texture_t* texture);
void xne_destroy_texture(xne_Texture_t* texture);

typedef struct xne_TextureAtlas {
    xne_Image_t image;
    
    uint32_t target;
    uint32_t texture_location, layer_location;
    xne_Enum_t filter, wrap;

    uint32_t tile_width, tile_height;
} xne_TextureAtlas_t;

int xne_create_texture_atlasf(xne_TextureAtlas_t* texture, FILE* file, const uint32_t tilex, const uint32_t tiley,
                           xne_TextureFilter_t filter, xne_TextureWrap_t wrap);
static inline int xne_create_texture_atlas(xne_TextureAtlas_t* texture, const char* path, const uint32_t tilex, const uint32_t tiley,
                           xne_TextureFilter_t filter, xne_TextureWrap_t wrap){
    FILE* f = fopen(path, "rb");
    int s = xne_create_texture_atlasf(texture, f, tilex, tiley, filter, wrap);
    fclose(f);
    return s;
}

int xne_link_texture_atlas(xne_TextureAtlas_t* texture, const char* name, uint32_t program);
int xne_link_texture_atlas_layer(xne_TextureAtlas_t* texture, const char* name, uint32_t program);
void xne_texture_atlas_enable(xne_TextureAtlas_t* texture, xne_TextureUnit_t unit, int x, int y);
void xne_texture_atlas_disable(xne_TextureAtlas_t* texture);
void xne_destroy_atlas_texture(xne_TextureAtlas_t* texture);

#endif