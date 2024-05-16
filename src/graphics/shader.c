#include "shader.h"

#include <memory.h>

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include <string.h>

#include <GL/glew.h>

#include "texture.h"

#define XNE_BUFFER_SIZE 1024

static xne_Shader_t* __current_shader = NULL;

xne_Shader_t* xne_get_active_shader(){ return __current_shader; }

static int xne__compile_shader(uint32_t* shader, GLenum __type, const char* __value){
    *shader = glCreateShader(__type);

    glShaderSource(*shader, 1, (char const * const *)&__value, NULL);
    glCompileShader(*shader);

    int s;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &s);
    if(!s){
        char buffer[XNE_BUFFER_SIZE];
        glGetShaderInfoLog(*shader, XNE_BUFFER_SIZE, NULL, buffer);
        fprintf(stderr, buffer);

        return XNE_FAILURE;
    }

    return XNE_OK;
}

static int xne__link_program(uint32_t __program){
    glLinkProgram(__program);

    int s;
    glGetProgramiv(__program, GL_LINK_STATUS, &s);
    if(!s){
        char buffer[XNE_BUFFER_SIZE];
        glGetProgramInfoLog(__program, XNE_BUFFER_SIZE, NULL, buffer);
        fprintf(stderr, buffer);

        return XNE_FAILURE;
    }

    return XNE_OK;
}

static void xne__free_program(uint32_t __program, const size_t __count){
    if(__count == 0) return;
    uint32_t* shaders = (uint32_t*) malloc(__count * sizeof(uint32_t));

    glGetAttachedShaders(__program, __count, NULL, shaders);
    for (size_t i = 0; i < __count; i++)
    {
        glDetachShader(__program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    
    free(shaders);
}

int xne_create_shaderf(xne_Shader_t* shader, FILE* file, const xne_ShaderDesc_t* desc){
    assert(shader);
    assert(desc);

    shader->count = 0;
    xne_ShaderDesc_t* shr = (xne_ShaderDesc_t*)&desc[0];
    while (shr->type != 0)
        shr = (xne_ShaderDesc_t*)&desc[++shader->count];

    assert(shader->count);

    shader->shaders = (uint32_t*) malloc(shader->count * sizeof(uint32_t));
    shader->program = glCreateProgram();

    shader->uniform_count = 0;
    shader->uniforms = (struct xne_ShaderUniform*) malloc(0);
    assert(shader->uniforms);

    fseek(file, 0, SEEK_SET);
    size_t fsize = xne_fsize(file);
    if(fsize == -1){
        fprintf(stderr, "Error while reading shader's file!\n");
        return XNE_FAILURE;
    }

    fseek(file, 0, SEEK_SET);
    char* fstr = (char*) calloc(fsize + 1, sizeof(char));
    size_t fsize0 = fread(fstr, sizeof(char), fsize , file);
    fstr[fsize0] = '\0';
    
    fseek(file, 0, SEEK_SET);
    int offset = 0, c;
    do {
        c = xne_freadw8(file);
        offset++;
    } while (c != EOF && c != '\n');

    int success;
    for (size_t i = 0; i < shader->count; i++)
    {
        /*std::string cpy = std::string(fstr);
        cpy = cpy.insert(offset, "#define " + std::string(desc[i].name));*/
        char* strh;
        xne_str_merge("#define", desc[i].name, strh);

        const char* buffer = (char*) xne_str_insert(fstr, strh, offset);

        success = xne__compile_shader(&shader->shaders[i], desc[i].type, buffer);
        if(success == XNE_FAILURE){
            fprintf(stderr, "failed to compile shader '%i'\n", i);
            return XNE_FAILURE;
        }
        
        glAttachShader(shader->program, shader->shaders[i]);
        
        free(strh);
        free((char*)buffer);
    }

    if(xne__link_program(shader->program) == XNE_FAILURE){
        fprintf(stderr, "failed to link program '%i'\n", shader->program);
        return XNE_FAILURE;
    }

    free(fstr);

    return XNE_OK;
}

int xne_create_shaderfv(xne_Shader_t* shader, const char* vert, const char* frag){
    shader->count = 2;
    shader->shaders = (uint32_t*) malloc(2 * sizeof(uint32_t));
    shader->program = glCreateProgram();

    xne__compile_shader(&shader->shaders[0], GL_VERTEX_SHADER, vert);
    xne__compile_shader(&shader->shaders[1], GL_FRAGMENT_SHADER, frag);
    glAttachShader(shader->program, shader->shaders[0]);
    glAttachShader(shader->program, shader->shaders[1]);

    xne__link_program(shader->program);
    return 0;
}

void xne_shader_enable(xne_Shader_t* shader) { 
    if(__current_shader == shader) return;

    __current_shader = shader;
    glUseProgram(shader->program); 
}

void xne_shader_disable(xne_Shader_t* shader) { 
    __current_shader = NULL;
    glUseProgram(0); 
}

int xne_link_shader_uniforms(xne_Shader_t* shader, const xne_ShaderUniformDesc_t* uniforms){
    if(uniforms == NULL){
        return XNE_FAILURE;
    }

    xne_ShaderUniformDesc_t* uniform = (xne_ShaderUniformDesc_t*)&uniforms[0];
    while (uniform->attrib != 0)
        uniform = (xne_ShaderUniformDesc_t*)&uniforms[++shader->uniform_count];

    if(!shader->uniform_count){
        return XNE_FAILURE;
    }
    
    shader->uniforms = (struct xne_ShaderUniform*) realloc(shader->uniforms, sizeof(struct xne_ShaderUniform) * shader->uniform_count);
    assert(shader->uniforms);

    for (size_t i = 0; i < shader->uniform_count; i++)
    {
        shader->uniforms[i].attrib = uniform[i].attrib;
        shader->uniforms[i].format = uniforms[i].format;
        shader->uniforms[i].location = glGetUniformLocation(shader->program, uniforms[i].name);
    }

    return XNE_OK;
}

void xne_shader_use_uniform(xne_Shader_t* shader, uint32_t index, const void* value){    
    if(index < shader->uniform_count){
        switch (shader->uniforms[index].format)
        {
        case XNE_UNIFORM_FLOAT: glUniform1fv(shader->uniforms[index].location, 1, (float*)value);   return;
        case XNE_UNIFORM_INT:   glUniform1iv(shader->uniforms[index].location, 1, (int*)value);     return;
        case XNE_UNIFORM_VEC2:  glUniform2fv(shader->uniforms[index].location, 1, (float*)value);   return;
        case XNE_UNIFORM_VEC3:  glUniform3fv(shader->uniforms[index].location, 1, (float*)value);   return;
        case XNE_UNIFORM_VEC4:  glUniform4fv(shader->uniforms[index].location, 1, (float*)value);   return;
        case XNE_UNIFORM_MAT4:  glUniformMatrix4fv(shader->uniforms[index].location, 1, GL_FALSE, (float*)value);   return;
        default: return;
        }
    }
}

int xne_shader_is_valid(xne_Shader_t* shader){
    if(!shader || !glIsProgram(shader->program) || shader->count <= 0) {
        return XNE_FAILURE;
    }

    for (size_t i = 0; i < shader->count; i++)
    {
        if(!glIsShader(shader->shaders[i])){
            return XNE_FAILURE;
        }
    }
    
    return XNE_OK;
}

void xne_destroy_shader(xne_Shader_t* shader){
    for (size_t i = 0; i < shader->count; i++)
    {
        glDetachShader(shader->program, shader->shaders[i]);
        glDeleteShader(shader->shaders[i]);
    }

    free(shader->shaders);
    glDeleteProgram(shader->program);
}