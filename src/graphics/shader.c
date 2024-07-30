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

    glShaderSource(*shader, 1, (const char * const *)&__value, NULL);
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
    assert(file);
    assert(desc);

    memset(shader, 0, sizeof(xne_Shader_t));

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
        const char* strh = xne_string_merge("#define ", desc[i].name);
        const char* buffer = (char*) xne_string_insert(fstr, strh, offset);

        success = xne__compile_shader(&shader->shaders[i], desc[i].type, buffer);
        if(success == XNE_FAILURE){
            fprintf(stderr, "failed to compile shader '%i'\n", i);
            return XNE_FAILURE;
        }
        
        glAttachShader(shader->program, shader->shaders[i]);
        
        free((char*) strh);
        free((char*) buffer);
    }

    if(xne__link_program(shader->program) == XNE_FAILURE){
        fprintf(stderr, "failed to link program '%i'\n", shader->program);
        return XNE_FAILURE;
    }

    free(fstr);

    return XNE_OK;
}

int xne_create_shaderfv(xne_Shader_t* shader, const char* vert, const char* frag){
    assert(shader);
    assert(vert && frag);

    memset(shader, 0, sizeof(xne_Shader_t));

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

static size_t xne__get_uniform_desc_length(const xne_ShaderUniformDesc_t* uniforms){
    size_t count = 0;
    xne_ShaderUniformDesc_t* ptr = (xne_ShaderUniformDesc_t*)&uniforms[0];
    while (ptr->attrib != 0)
    {
        ptr = (xne_ShaderUniformDesc_t*)&uniforms[++count];
    }

    return count;
}

int xne_link_shader_uniforms(xne_Shader_t* shader, const xne_ShaderUniformDesc_t* uniforms){
    assert(shader);

    if(uniforms == NULL){
        return XNE_FAILURE;
    }

    size_t count = xne__get_uniform_desc_length(uniforms);
    xne_vprintf("count %i", count);

    if(!count){
        return XNE_FAILURE;
    }
    
    if(shader->uniform_count > 0){
        shader->uniform_count += count;
        shader->uniforms = realloc(shader->uniforms, sizeof(struct xne_ShaderUniform) * shader->uniform_count);
    }
    else {
        shader->uniform_count = count;
        shader->uniforms = malloc(sizeof(struct xne_ShaderUniform) * shader->uniform_count);
    }

    xne_assert(shader->uniforms);
    //memset(shader->uniforms, 0, sizeof(struct xne_ShaderUniform) * shader->uniform_count);

    for (size_t i = 0; i < shader->uniform_count; i++)
    {
        shader->uniforms[i].attrib = (xne_UniformAttrib_t) uniforms[i].attrib;
        shader->uniforms[i].format = (xne_UniformType_t) uniforms[i].format;
        shader->uniforms[i].location = glGetUniformLocation(shader->program, uniforms[i].name);

        shader->uniforms[i].length = 1;
        if(uniforms[i].length != XNE_INVALID_VALUE){
            //shader->uniforms[i].length = (uint32_t) uniforms[i].length;
        }

        /*shader->uniforms[i].location = -1;
        if(shader->uniforms[i].attrib & XNE_UNIFORM_ATTRIB_STRUCT) {
            shader->uniforms[i].location = glGetUniformLocation(shader->program, uniforms[i].name);
        }
        if(shader->uniforms[i].attrib & XNE_UNIFORM_ATTRIB_ARRAY) {
            //const char* temp = xne_string_merge((const char*) uniforms[i].name, "[0]");
            //shader->uniforms[i].location = glGetUniformLocation(shader->program, temp);
            //free((char*) temp);
        }*/

        /*xne_vprintf("cannot find attribute '%i'", uniform[i].attrib);
        if(shader->uniforms[i].attrib == XNE_UNIFORM_ATTRIB_UNIFORM) {
            shader->uniforms[i].location = glGetUniformLocation(shader->program, uniforms[i].name);
        }*/

        if(shader->uniforms[i].location == XNE_INVALID_VALUE){
            xne_vprintf("cannot find attribute '%s'", uniforms[i].name);
        }
    }

    return XNE_OK;
}

void xne_shader_use_uniform(xne_Shader_t* shader, uint32_t index, const void* value){    
    if(index < shader->uniform_count){
        if(shader->uniforms[index].location == XNE_INVALID_VALUE){
            return;
        }

        switch (shader->uniforms[index].format)
        {
        case XNE_UNIFORM_FLOAT: 
            glUniform1fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   
            break;

        case XNE_UNIFORM_INT:   
            glUniform1iv(shader->uniforms[index].location, shader->uniforms[index].length, (int*)value);     
            break;

        case XNE_UNIFORM_VEC2:  
            glUniform2fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   
            break;

        case XNE_UNIFORM_VEC3:  
            glUniform3fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   
            break;

        case XNE_UNIFORM_VEC4:  
            glUniform4fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   
            break;

        case XNE_UNIFORM_MAT4:  
            glUniformMatrix4fv(shader->uniforms[index].location, shader->uniforms[index].length, GL_FALSE, (float*)value);   
            break;

        case XNE_UNIFORM_LIGHT:

            break;
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