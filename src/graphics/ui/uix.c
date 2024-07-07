#include "uix.h"

#include "engine/engine.h"
#define XNE_INCLUDE_MESH_GEN
#include "graphics/graphics.h"

#include "core/math.h"

#include <string.h>
#include <memory.h>
#include <GL/glew.h>

#define XNE_SHADER_VERSION "#version 440\n"
#define XNE_DEFAULT_PIXEL_BUFFER_SIZE 512

static xne_UserInterface_t __context;

static void xne__clear_texture_buffer(char r, char g, char b) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, __context.pixel_object[0]);

    char* ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if(ptr){
        for (size_t i = 0; i < __context.memory.size; i += 4)
        {
            ptr[i] = r;
            ptr[i + 1] = g;
            ptr[i + 2] = b;
            ptr[i + 3] = 255;
        }

        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glBindTexture(GL_TEXTURE_2D, __context.texture_object);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sqrt(__context.memory.size / 4), sqrt(__context.memory.size / 4), GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void xne__enable_shader(float* color){
    glUseProgram(__context.shader.dev);

    if(color){
        glUniform3fv(__context.shader.color_uniform, 1, color);
    }

    glUniformMatrix4fv(__context.shader.matrix_uniform, 1, GL_FALSE, &__context.projection[0][0]);
}

static void xne__disable_shader(){
    glUseProgram(0);
}

int xne_create_uix_instance(size_t default_buffer_size){
    if(!__context.vertex_object){
        xne_destroy_uix_instance();
    }

    if(default_buffer_size == 0){
        default_buffer_size = XNE_DEFAULT_PIXEL_BUFFER_SIZE;
    }

    {
        uint32_t status;
        const char* vert_shr = ""
            XNE_SHADER_VERSION
            "layout(location = 0) in vec4 vertex;\n"
            "uniform mat4 projection;\n"
            "uniform vec3 color;\n"
            "out vec2 Frag_Uv;\n"
            "out vec3 Frag_Color;\n"
            "void main(){\n"
            "   gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
            "   Frag_Uv = vertex.zw;\n"
            "   Frag_Color = color;\n"
            "}\n";

        const char* frag_shr = ""
            XNE_SHADER_VERSION
            "in vec2 Frag_Uv;\n"
            "in vec3 Frag_Color;\n"
            "uniform sampler2D image;\n"
            "void main(){\n"
            "   vec4 sampled = texture2D(image, Frag_Uv);"
            "   gl_FragColor = vec4(sampled.xyz, 1.0);\n"
            "}\n";

        __context.shader.dev = glCreateProgram();
        __context.shader.vert = glCreateShader(GL_VERTEX_SHADER);
        __context.shader.frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(__context.shader.vert, 1, &vert_shr, 0);
        glShaderSource(__context.shader.frag, 1, &frag_shr, 0);
        glCompileShader(__context.shader.vert);
        glCompileShader(__context.shader.frag);

        glGetShaderiv(__context.shader.vert, GL_COMPILE_STATUS, &status);
        assert(status == GL_TRUE);
        glGetShaderiv(__context.shader.frag, GL_COMPILE_STATUS, &status);
        assert(status == GL_TRUE);
        
        glAttachShader(__context.shader.dev, __context.shader.vert);
        glAttachShader(__context.shader.dev, __context.shader.frag);
        glLinkProgram(__context.shader.dev);

        glGetProgramiv(__context.shader.dev, GL_LINK_STATUS, &status);
        assert(status == GL_TRUE);

        __context.shader.vertex_attrib = glGetAttribLocation(__context.shader.dev, "vertex");

        __context.shader.matrix_uniform = glGetUniformLocation(__context.shader.dev, "projection");
        __context.shader.color_uniform = glGetUniformLocation(__context.shader.dev, "color");
        __context.shader.texture_uniform = glGetUniformLocation(__context.shader.dev, "image");
        fprintf(stdout, "texid %i\n", __context.shader.texture_uniform);
    }

    {
        glGenVertexArrays(1, &__context.vertex_object);
        glGenBuffers(1, &__context.buffer_object);
        glBindVertexArray(__context.vertex_object);
        glBindBuffer(GL_ARRAY_BUFFER, __context.buffer_object);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(__context.shader.vertex_attrib);
        glVertexAttribPointer(__context.shader.vertex_attrib, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    __context.scissors.x = 0;
    __context.scissors.y = 0;
    __context.scissors.width = default_buffer_size;
    __context.scissors.height = default_buffer_size;

    {
        // intialize shared texture data
        __context.memory.elemsize = sizeof(char);
        __context.memory.size = default_buffer_size * default_buffer_size * 4;
        __context.memory.ptr = calloc(__context.memory.size, sizeof(char));
        assert(__context.memory.ptr);
        memset(__context.memory.ptr, 0, __context.memory.size);

        // create a new texture 2d
        glGenTextures(1, &__context.texture_object);
        glBindTexture(GL_TEXTURE_2D, __context.texture_object);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, default_buffer_size, default_buffer_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, __context.memory.ptr);

        // create a new pixel object buffer
        glGenBuffers(2, &__context.pixel_object[0]);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, __context.pixel_object[0]);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, __context.pixel_object[1]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, __context.memory.size, NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_PIXEL_PACK_BUFFER, __context.memory.size, NULL, GL_DYNAMIC_READ);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return XNE_OK;
}

void xne_uix_new_frame(uint32_t width, uint32_t height){
    xne__clear_texture_buffer(255, 0, 0);

    xne_orthographic_projection_off_center(0, width, height, 0.0, 0.0f, 1.0f, 1.0f, __context.projection);
}

void xne_draw_font_atlas(xne_Font_t* font){
    glBindVertexArray(__context.vertex_object);
    glBindBuffer(GL_ARRAY_BUFFER, __context.buffer_object);

    if(1){
        const xne_vec3i color = {255, 255, 255};
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, __context.pixel_object[0]);

        char* dbuffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        if(dbuffer){
            for (size_t y = 0; y < font->atlas.height; y++)
            {
                dbuffer[y + 1] = 255;
                for (size_t x = 0; x < font->atlas.width; x++)
                {
                    
                }
            }
            
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        }

        glBindTexture(GL_TEXTURE_2D, __context.texture_object);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
            sqrt(__context.memory.size / 4), 
            sqrt(__context.memory.size / 4),
            GL_RGBA, GL_UNSIGNED_BYTE, NULL
        );

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, __context.texture_object);
    
    const float position_x = 0.0f;
    const float position_y = 0.0f;
    const float width = font->atlas.width;
    const float height = font->atlas.height;

    float vertices[24] = {
        position_x - width, position_y + height, 0, 1,
        position_x - width, position_y - height, 0, 0,
        position_x + width, position_y - height, 1, 0,
        position_x - width, position_y + height, 0, 1,
        position_x + width, position_y - height, 1, 0,
        position_x + width, position_x + height, 1, 1
    };

    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if(ptr){
        memcpy(ptr, vertices, 24 * sizeof(float));

        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
    
    xne__enable_shader(NULL);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    xne__disable_shader();
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void xne_draw_text_scaled(xne_Font_t* font, const char* text, float x, float y, float scale, xne_Colorf_t color){
    /*xne_shader_enable(&font->shader);
    xne_shader_use_uniform(&font->shader, 0, NULL);
    xne_shader_use_uniform(&font->shader, 1, color);

    glBindVertexArray(__context.vertex_object);
    glActiveTexture(GL_TEXTURE0);

    for (size_t c = 0; c < strlen(text); c++)
    {
        xne_Glyph_t* glyph = (xne_Glyph_t*) xne_vector_get(&font->glyphs, text[c]);

        float position_x = x + glyph->offset_x * scale;
        float position_y = y - (glyph->height - glyph->offset_y) * scale;

        float width = glyph->width * scale;
        float height = glyph->height * scale;

        float anchor_x = (float)glyph->pen_x / font->atlas.width;
        float anchor_y = (float)glyph->pen_y / font->atlas.height;
        float anchor_max_x = (float)(glyph->pen_x + glyph->width) / font->atlas.width;
        float anchor_max_y = (float)(glyph->pen_y + glyph->height) / font->atlas.height;

        float vertices[24] = {
            position_x, position_y + height, anchor_x, anchor_y,
            position_x, position_y, anchor_x, anchor_max_y,
            position_x + width, position_y, anchor_max_x, anchor_max_y,

            position_x, position_y + height, anchor_x, anchor_y,
            position_x + width, position_y, anchor_max_x, anchor_max_y,
            position_x + width, position_y + height, anchor_max_x, anchor_y
        };

        glBindTexture(GL_TEXTURE_2D, font->atlas.target);
        glBindBuffer(GL_ARRAY_BUFFER, __context.buffer_object);

        void* buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(buffer, vertices, sizeof(vertices));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x+= glyph->advance * scale; 
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);*/
}

void xne_destroy_uix_instance(){
    glDeleteVertexArrays(1, &__context.vertex_object);
    glDeleteBuffers(1, &__context.buffer_object);
    glDeleteBuffers(2, &__context.pixel_object[0]);
    glDeleteTextures(1, &__context.texture_object);

    if(__context.shader.dev){
        glDeleteShader(__context.shader.vert);
        glDeleteShader(__context.shader.frag);
        glDeleteProgram(__context.shader.dev);
    }

    free(__context.memory.ptr);

    memset(&__context, 0, sizeof(xne_UserInterface_t));
}