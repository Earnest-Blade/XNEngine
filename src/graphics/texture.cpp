#include "texture.h"

#define XNE_CORE_FILE
#include "core/core.h"

#include <assert.h>
#include <memory.h>
#include <GL/glew.h>

#if defined(XNE_NO_PNG) && defined(XNE_NO_BMP)
    #error Unsupported format : Any image format selected!
#endif

#ifndef XNE_NO_PNG

#include <png.h>

#define XNE_PNG_HEADER_SIZE 8

static int xne__is_png(FILE* __file){
    size_t curr = ftell(__file);

    fseek(__file, 0, SEEK_SET);
    uint8_t header[XNE_PNG_HEADER_SIZE];
    fread(header, 1, XNE_PNG_HEADER_SIZE, __file);
    
    fseek(__file, curr, SEEK_SET);

    return !png_sig_cmp(header, 0, XNE_PNG_HEADER_SIZE);
}

static void xne__png_error(png_structp sptr, png_const_charp cc){
    fprintf(stderr, "%s\n", cc);
}

static int xne__load_png(xne_Image_t* __dest, FILE* __file){
    __dest->type = XNE_PNG;
    __dest->alignment = 1;

    png_structp pngldr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, xne__png_error, NULL);
    assert(pngldr);

    png_infop pnginfo = png_create_info_struct(pngldr);
    assert(pnginfo);

    if(setjmp(png_jmpbuf(pngldr))){
        png_destroy_read_struct(&pngldr, &pnginfo, NULL);
        return 0;
    }

    fseek(__file, XNE_PNG_HEADER_SIZE, SEEK_SET);

    png_init_io(pngldr, __file);
    png_set_sig_bytes(pngldr, XNE_PNG_HEADER_SIZE);
    png_read_info(pngldr, pnginfo);
    //png_get_IHDR(pngldr, pnginfo, (uint32_t*)&__dest->width, (uint32_t*)&__dest->height, &__dest->depth, 
    //            &clrType, NULL, NULL, NULL);

    __dest->width = png_get_image_width(pngldr, pnginfo);
    __dest->height = png_get_image_width(pngldr, pnginfo);
    int clrType = png_get_color_type(pngldr, pnginfo);
    __dest->depth = png_get_bit_depth(pngldr, pnginfo);

    if(clrType == PNG_COLOR_TYPE_PALETTE){
        png_set_palette_to_rgb(pngldr);
    }
    if(clrType == PNG_COLOR_TYPE_GRAY && __dest->depth < 8){
        png_set_expand_gray_1_2_4_to_8(pngldr);
    }
    if(png_get_valid(pngldr, pnginfo, PNG_INFO_tRNS)){
        png_set_tRNS_to_alpha(pngldr);
    }

    if(__dest->depth == 16){
        png_set_strip_16(pngldr);
    }
    else if(__dest->depth < 8){
        png_set_packing(pngldr);
    }

    png_read_update_info(pngldr, pnginfo);
    clrType = png_get_color_type(pngldr, pnginfo);

    switch (clrType)
    {
    case PNG_COLOR_TYPE_RGBA:
        __dest->format = XNE_RGBA;
        __dest->channels = 4;
        break;
    case PNG_COLOR_TYPE_RGB:
        __dest->format = XNE_RGB;
        __dest->channels = 3;
        break;

    default:
        fprintf(stderr, "Color type %i not supported!\n", clrType);
        png_destroy_read_struct(&pngldr, &pnginfo, NULL);
        return 0;
    }
    fprintf(stdout, "image used %i channels!\n", __dest->channels);

    size_t rowbytes = png_get_rowbytes(pngldr, pnginfo);
    __dest->pixels = (uint8_t*) malloc(__dest->width * __dest->height * __dest->channels * sizeof(uint8_t));

    if(!__dest->pixels){
        fprintf(stderr, "Cannot alloc pixels\n", clrType);
        png_destroy_read_struct(&pngldr, &pnginfo, NULL);
        // alloc error
        return 0;
    }

    uint8_t** rowp = (uint8_t**) malloc(__dest->height * sizeof(uint8_t*));
    if(!rowp){
        fprintf(stderr, "Cannot alloc rowp\n", clrType);
        png_destroy_read_struct(&pngldr, &pnginfo, NULL);
        // alloc error
        return 0;
    }

    for (size_t i = 0; i < __dest->height; i++)
    {
        rowp[__dest->height - 1 - i] = __dest->pixels + i * rowbytes;
    }

    png_read_image(pngldr, rowp);

    free(rowp);
    return __dest->pixels != NULL;
}

#endif

#ifndef XNE_NO_BMP

static int xne__is_bmp_raw(FILE* __file){
    int sz;
    if(xne_freadw8(__file) != 'B') return 0;
    if(xne_freadw8(__file) != 'M') return 0;
    xne_freadw32(__file); // fsize
    xne_freadw16(__file); // res1
    xne_freadw16(__file); // res2
    xne_freadw32(__file); // offset
    sz = xne_freadw32(__file);
    return (sz == 12 || sz == 40 || sz == 56 || sz == 108 || sz == 124);
}

static bool xne__is_bmp(FILE* __file){
    size_t curr = ftell(__file);
    fseek(__file, 0, SEEK_SET);
    bool result = xne__is_bmp_raw(__file);
    fseek(__file, curr, SEEK_SET);
    return result;
}

static int xne__load_bmp(xne_Image_t* __dest, FILE* __file){
    __dest->type = XNE_BMP;
    __dest->format = XNE_BGR;
    __dest->alignment = 4;
    __dest->channels = 3;
    
    xne_freadw16(__file); // type;
    int fisze = xne_freadw32(__file); // size;
    xne_freadw16(__file); // res1
    xne_freadw16(__file); // res2

    uint32_t offset = xne_freadw32(__file);
    uint32_t size = xne_freadw32(__file);
    __dest->width = xne_fread32(__file);
    __dest->height = xne_fread32(__file);

    xne_freadw16(__file); // planes;
    __dest->depth = xne_freadw16(__file);
    xne_freadw32(__file); // compression type
    uint32_t isize = xne_freadw32(__file);

    xne_fread32(__file); // x per meters
    xne_fread32(__file); // y per meters
    uint32_t clrType = xne_freadw32(__file); // color used
    xne_freadw32(__file); // color important

    if(isize == 0) isize = __dest->width * __dest->height * __dest->channels;
    if(offset == 0) offset = 54;

    __dest->pixels = (uint8_t*) malloc(isize * sizeof(uint8_t));
    if(!__dest->pixels){
        fprintf(stderr, "Cannot alloc pixels!\n");
        // alloc error
        return 0;
    }

    fseek(__file, offset, SEEK_SET);
    fread(__dest->pixels, sizeof(uint8_t), isize, __file);
    return __dest->pixels != NULL;
}

#endif

int xne_create_image(xne_Image_t* dest, FILE* file){
    assert(dest);
    assert(file);
    fseek(file, 0, SEEK_SET);

    int status = 0;
    
#ifndef XNE_NO_PNG
    if(xne__is_png(file)) status = xne__load_png(dest, file);
#endif

#ifndef XNE_NO_BMP
    if(xne__is_bmp(file) && !status) status = xne__load_bmp(dest, file);
#endif

#ifndef XNE_NO_FLIP
    xne_flip_image_vertically(dest);
#endif
    return status;
}

void xne_flip_image_vertically(xne_Image_t* src){
    int row;
    size_t stride = src->width * src->channels;
    uint8_t buffer[2048];
    uint8_t* bytes = src->pixels;

    for (row = 0; row < (src->height>>1); row++)
    {
        uint8_t* row0 = bytes + row * stride;
        uint8_t* row1 = bytes + (src->height - row - 1) * stride;

        size_t bleft = stride;
        while (bleft)
        {
            size_t bcpy = (bleft < sizeof(buffer)) ? bleft : sizeof(buffer);
            memcpy(buffer, row0, bcpy);
            memcpy(row0, row1, bcpy);
            memcpy(row1, buffer, bcpy);
            row0 += bcpy;
            row1 += bcpy;
            bleft -= bcpy;
        }
    }
}

void xne_destroy_image(xne_Image_t* src){
    if(!src) return;
    if(src->pixels) free(src->pixels);
}

static xne_TextureSizedFormat_t xne__get_sized_format(xne_TextureFormat_t format){
    switch (format)
    {
    case XNE_RED: return XNE_RED8;
    case XNE_RG: return XNE_RG8;
    case XNE_RGB: case XNE_BGR: return XNE_RGB8;
    case XNE_RGBA: case XNE_BGRA: return XNE_RGBA8;
    default:
        return XNE_RED8;
    }
}

int xne_create_texturef(xne_Texture_t* texture, FILE* file, xne_TextureFilter_t filter, xne_TextureWrap_t wrap){
    assert(texture);
    assert(file);
    texture->filter = filter;
    texture->wrap = wrap;
    texture->target = 0;

    /* loading image */
    xne_create_image(&texture->image, file);
    if(!texture->image.pixels) {
        fprintf(stderr, "invalid image!\n");
        return 0;
    }

    glGenTextures(1, &texture->target);
    glBindTexture(GL_TEXTURE_2D, texture->target);

    glPixelStorei(GL_UNPACK_ALIGNMENT, texture->image.alignment);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)texture->wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)texture->wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)texture->filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)texture->filter);

    glTexImage2D(GL_TEXTURE_2D, 0, texture->image.channels, texture->image.width, 
                texture->image.height, 0, texture->image.format, GL_UNSIGNED_BYTE, texture->image.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    free(texture->image.pixels);
    return 1;
}

int xne_link_texture(xne_Texture_t* texture, const char* name, uint32_t program){
    assert(texture);
    
    texture->texture_location = glGetUniformLocation(program, name);
    if(texture->texture_location == -1){
        fprintf(stderr, "failed to link texture '%s' to shader '%i'!\n", name, program);
        return XNE_FAILURE;
    }

    return XNE_OK;
}

void xne_texture_enable(xne_Texture_t* texture, xne_TextureUnit_t unit){    
    if(texture->texture_location >= 0){
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, texture->target);
        glUniform1i(texture->texture_location, XNE_TEXTURE_UNIT0 - unit);
    }
}

void xne_texture_disable(xne_Texture_t* texture){
    glBindTexture(GL_TEXTURE_2D, 0);
}

void xne_destroy_texture(xne_Texture_t* texture){
    if(texture->target > 0){
        glDeleteTextures(1, &texture->target);
    }
    xne_destroy_image(&texture->image);
}

int xne_create_texture_atlasf(xne_TextureAtlas_t* texture, FILE* file, const uint32_t tilex, const uint32_t tiley,
                           xne_TextureFilter_t filter, xne_TextureWrap_t wrap){
    assert(texture);
    assert(file);
    texture->filter = filter;
    texture->wrap = wrap;

    xne_create_image(&texture->image, file);
    if(!texture->image.pixels) {
        fprintf(stderr, "invalid image!\n");
        return 0;
    }

    texture->tileWidth = texture->image.width / tilex;
    texture->tileHeight = texture->image.height / tiley;

    glGenTextures(1, &texture->target);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture->target);
    glPixelStorei(GL_UNPACK_ALIGNMENT, texture->image.alignment);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 1);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, (int)texture->wrap);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, (int)texture->wrap);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, (int)texture->filter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (int)texture->filter);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, xne__get_sized_format(texture->image.format), texture->tileWidth, texture->tileHeight, tilex * tiley);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, texture->image.width);
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, texture->image.height);

    for (size_t y = 0; y < tiley; y++)
    {
        for (size_t x = 0; x < tilex; x++)
        {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0,
            y * tilex + x, texture->tileWidth, texture->tileHeight, 1, texture->image.format, GL_UNSIGNED_BYTE,
            &texture->image.pixels[(y * texture->tileHeight * texture->image.width + x * texture->tileWidth) * texture->image.channels]);
        }
    }
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    free(texture->image.pixels);
}

int xne_link_texture_atlas(xne_TextureAtlas_t* texture, const char* name, uint32_t program){
    texture->texLocation = glGetUniformLocation(program, name);
    if(texture->texLocation == -1){
        fprintf(stderr, "failed to link the shader!\n");
        return 0;
    }
    return 1;
}

int xne_link_texture_atlas_layer(xne_TextureAtlas_t* texture, const char* name, uint32_t program){
    texture->layerLocation = glGetUniformLocation(program, name);
    if(texture->layerLocation == -1){
        fprintf(stderr, "failed to link the altas layer uniform to the shader!\n");
        return 0;
    }
    return 1;
}

void xne_texture_atlas_enable(xne_TextureAtlas_t* texture, xne_TextureUnit_t unit, int x, int y){
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture->target);
    glUniform1i(texture->texLocation, XNE_TEXTURE_UNIT0 - unit);
    glUniform1i(texture->layerLocation, y * (texture->image.height / texture->tileHeight) + x);
}

void xne_texture_atlas_disable(xne_TextureAtlas_t* texture){
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void xne_destroy_atlas_texture(xne_TextureAtlas_t* texture){
    if(texture->target > 0){
        glDeleteTextures(1, &texture->target);
    }
    xne_destroy_image(&texture->image);
}