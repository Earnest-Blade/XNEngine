#include "font.h"

#include "graphics/shader.h"

#include <assert.h>
#include <memory.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <gl/glew.h>

static FT_Library __ft_instance;

static int xne__create_default_shader(xne_Font_t* font){
    const char* vert_shr = ""
        "#version 400\n"
        "layout(location = 0) in vec4 vertex;\n"
        "uniform mat4 projection;\n"
        "out vec2 out_uv;\n"
        "void main(){\n"
        "   gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "   out_uv = vertex.zw;\n"
        "}\n";
    
    const char* frag_shr = ""
        "#version 400\n"
        "in vec2 out_uv;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 color;\n"
        "void main(){\n"
        "   vec4 sample_a = vec4(1.0, 1.0, 1.0, texture(text, out_uv).r);"
        "   gl_FragColor = vec4(out_uv.xy, 0.0, 1.0) * sample_a;"
        "}\n";

    if(xne_create_shaderfv(&font->shader, vert_shr, frag_shr) != XNE_OK){
        return XNE_FAILURE;
    }

    const xne_ShaderUniformDesc_t shader_uniforms[] = {
        {1, XNE_UNIFORM_MAT4, "projection"},
        {1, XNE_UNIFORM_VEC3, "color"},
        XNE_SHADER_UNIFORM_END()
    };

    xne_link_shader_uniforms(&font->shader, shader_uniforms);

    return XNE_OK;
}

int xne_create_font(xne_Font_t* font, const char* path, int size){
    assert(font);
    assert(path);
    
    FT_Init_FreeType(&__ft_instance);

    FT_Face face;

    if(FT_New_Face(__ft_instance, path, 0, &face)){
        fprintf(stderr, "failed to load font!\n");
        return XNE_FAILURE;
    }

    font->size = size;
    font->thickness = 0.0f;

    FT_Set_Pixel_Sizes(face, size * 2.0f, size);

#if XNE_MAX_GLYPHS < 0
    xne_create_fixed_vector(&font->glyphs, sizeof(xne_Glyph_t), face->num_glyphs);

#elif XNE_MAX_GLYPHS == 0
    fprintf(stdout, "'XNE_MAX_GLYPHS == 0', no glyph has been loaded!\n");
    FT_Done_Face(face);
    FT_Done_FreeType(__ft_instance);
    
    return;
#else
    xne_create_fixed_vector(&font->glyphs, sizeof(xne_Glyph_t), XNE_MAX_GLYPHS);
#endif
    
    const int dimension = ((1 + face->size->metrics.height >> 6) * ceilf(sqrtf(font->glyphs.count)));
    font->atlas.width = 1;
    while(font->atlas.width < dimension) font->atlas.width <<= 1;
    font->atlas.height = font->atlas.width;

    uint8_t* buffer = calloc(font->atlas.width * font->atlas.height, sizeof(uint8_t));
    assert(buffer);

    // font atlas generation
    uint32_t pen_x = 0;
    uint32_t pen_y = 0;
    xne_Glyph_t* glyph;
    for (size_t i = 0; i < font->glyphs.count; i++)
    {
        glyph = (xne_Glyph_t*) xne_vector_get(&font->glyphs, i);

        if(FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT) || !face->glyph->bitmap.buffer){
            continue;
        }

        if(pen_x + face->glyph->bitmap.width >= font->atlas.width){
            pen_x = 0;
            pen_y += (face->size->metrics.height >> 6) + 1;
        }
        
        for (size_t row = 0; row < face->glyph->bitmap.rows; ++row)
        {
            for (size_t col = 0; col < face->glyph->bitmap.width; ++col)
            {
                buffer[(pen_y + row) * font->atlas.width + (pen_x + col)] 
                    = face->glyph->bitmap.buffer[row * face->glyph->bitmap.pitch + col];
            }
        }
        
        glyph->width = face->glyph->bitmap.width;
        glyph->height = face->glyph->bitmap.rows;
        glyph->pen_x = pen_x;
        glyph->pen_y = pen_y;
        glyph->offset_x = face->glyph->bitmap_left;
        glyph->offset_y = face->glyph->bitmap_top;
        glyph->advance = face->glyph->advance.x >> 6;
        glyph->target = font->atlas.target;

        pen_x += face->glyph->bitmap.width + 1;
    }

    glGenTextures(0, &font->atlas.target);
    glBindTexture(GL_TEXTURE_2D, font->atlas.target);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, font->atlas.width, font->atlas.height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    FT_Done_Face(face);
    FT_Done_FreeType(__ft_instance);

    xne__create_default_shader(font);

    return XNE_OK;
}

void xne_destroy_font(xne_Font_t* font){
    xne_destroy_vector(&font->glyphs);
    xne_destroy_shader(&font->shader);

    glDeleteTextures(1, &font->atlas.target);
}