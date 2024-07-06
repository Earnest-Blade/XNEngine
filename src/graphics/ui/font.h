#pragma

#ifndef XNE_FONT_IMPL
#define XNE_FONT_IMPL

#define XNE_CORE_FILE
#define XNE_CORE_BUFFER
#include "core/core.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

#ifndef XNE_MAX_GLYPHS
    #define XNE_MAX_GLYPHS -1
#endif

typedef struct xne_Glyph {
    uint16_t width, height;
    uint16_t offset_x, offset_y;
    uint16_t pen_x, pen_y;
    uint16_t advance;    
} xne_Glyph_t;

typedef struct xne_FontAtlas {
    struct xne_Buffer data;
    size_t width, height;
} xne_FontAtlas_t;

typedef struct xne_Font {
    xne_FontAtlas_t atlas;
    xne_Shader_t shader;
    xne_Vector_t glyphs;
    uint16_t size, thickness;
} xne_Font_t;

int xne_create_font(xne_Font_t* font, const char*, int size);

void xne_destroy_font(xne_Font_t* font);

static inline size_t xne_font_get_glyph_count(xne_Font_t* font) { return font->glyphs.count; }

#endif