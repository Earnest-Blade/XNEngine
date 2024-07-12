#include "shader.h"

#include <memory.h>

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include <string.h>

#include <GL/glew.h>

#include "light.h"
#include "texture.h"
#include "material.h"

#define XNE_BUFFER_SIZE 1024

static struct {
    xne_Shader_t* current;
    uint8_t binding_points;
} __shaders_state;

xne_Shader_t* xne_get_active_shader(){ return __shaders_state.current; }

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
    if(__shaders_state.current == shader) return;

    __shaders_state.current = shader;
    glUseProgram(shader->program); 
}

void xne_shader_disable(xne_Shader_t* shader) { 
    __shaders_state.current = NULL;
    glUseProgram(0); 
}

static size_t xne__get_uniform_count(const xne_ShaderUniformDesc_t* uniforms){
    int count = 0;
    xne_ShaderUniformDesc_t* uniform = (xne_ShaderUniformDesc_t*)&uniforms[0];
    
    while (uniform->attrib != XNE_UNIFORM_ATTRIB_NONE)
        uniform = (xne_ShaderUniformDesc_t*)&uniforms[++count];
    
    return count;
}

int xne_link_shader_uniforms(xne_Shader_t* shader, const xne_ShaderUniformDesc_t* uniforms){
    assert(shader);

    if(uniforms == NULL){
        return XNE_FAILURE;
    }

    size_t count = xne__get_uniform_count(uniforms);

    // when there is no uniform added.
    if(!count){
        return XNE_FAILURE;
    }

    if(shader->uniform_count + count > XNE_MAX_UNIFORM){
        xne_printf("cannot add new uniforms : the maximum number of uniforms is reached!");
        return XNE_FAILURE;
    }
    
    if(shader->uniform_count > 0){
        shader->uniforms = realloc(shader->uniforms, sizeof(struct xne_ShaderUniform) * shader->uniform_count);
    }
    else {
        shader->uniforms = malloc(sizeof(struct xne_ShaderUniform) * shader->uniform_count);
        shader->uniform_count = 0;
    }

    shader->uniform_count += count;

    xne_assert(shader->uniforms);
    memset(shader->uniforms, 0, sizeof(struct xne_ShaderUniform) * shader->uniform_count);

    for (size_t i = 0; i < shader->uniform_count; i++)
    {
        shader->uniforms[i].attrib = (xne_UniformAttrib_t) uniforms[i].attrib;
        shader->uniforms[i].format = (xne_UniformType_t) uniforms[i].format;
        
        // here, the order is important ; because of the flag's type value have to be overwrited.
        // first we check if this is a standard uniform
        // then, if this is an array based uniform
        // after if this is a struct uniform ; struct based uniforms, 
        //      because of blocks typed bindings, they overwrite other ways to define themselfs.

        shader->uniforms[i].location = XNE_INVALID_VALUE;
        if(shader->uniforms[i].attrib == XNE_UNIFORM_ATTRIB_UNIFORM){
            shader->uniforms[i].location = glGetUniformLocation(shader->program, uniforms[i].name);
            shader->uniforms[i].length = 1;
        }

        if((shader->uniforms[i].attrib == XNE_UNIFORM_ATTRIB_ARRAY)) {
            shader->uniforms[i].location = glGetUniformLocation(shader->program, uniforms[i].name);
            shader->uniforms[i].length = (uint16_t) uniforms[i].length;
        }

        if((shader->uniforms[i].attrib == XNE_UNIFORM_ATTRIB_STRUCT)) {
            shader->uniforms[i].length = (uint16_t) uniforms[i].length;
            xne_assert(shader->uniforms[i].length > 0);

            if(__shaders_state.binding_points + 1 >= XNE_MAX_BLOCK_UNIFORM){
                xne_printf("cannot add new blocked uniforms : the maximum number of binding points is reached!");
                return XNE_FAILURE;
            }

            glGenBuffers(1, &shader->uniforms[i].location);
            glBindBuffer(GL_UNIFORM_BUFFER, shader->uniforms[i].location);
            glBufferData(GL_UNIFORM_BUFFER, shader->uniforms[i].length, NULL, GL_STATIC_DRAW);

            uint32_t ubi = glGetUniformBlockIndex(shader->program, uniforms[i].name);
            if(ubi == XNE_INVALID_VALUE){
                xne_vprintf("cannot find uniform block '%s'", uniforms[i].name);
            }

            shader->uniforms[i].binding_point = __shaders_state.binding_points++;
            glUniformBlockBinding(shader->program, ubi, shader->uniforms[i].binding_point);
            glBindBufferBase(GL_UNIFORM_BUFFER, shader->uniforms[i].binding_point, shader->uniforms[i].location);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        if(shader->uniforms[i].location == XNE_INVALID_VALUE){
            xne_vprintf("cannot find uniform '%s'", uniforms[i].name);
        }
    }

    return XNE_OK;
}

void xne_shader_use_uniform(xne_Shader_t* shader, uint32_t index, const void* value){    
    if(index < shader->uniform_count && index != XNE_INVALID_VALUE && value){
        switch (shader->uniforms[index].format)
        {
        case XNE_UNIFORM_FLOAT: glUniform1fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   return;
        case XNE_UNIFORM_INT:   glUniform1iv(shader->uniforms[index].location, shader->uniforms[index].length, (int*)value);     return;
        case XNE_UNIFORM_VEC2:  glUniform2fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   return;
        case XNE_UNIFORM_VEC3:  glUniform3fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   return;
        case XNE_UNIFORM_VEC4:  glUniform4fv(shader->uniforms[index].location, shader->uniforms[index].length, (float*)value);   return;
        case XNE_UNIFORM_MAT4:  glUniformMatrix4fv(shader->uniforms[index].location, shader->uniforms[index].length, GL_FALSE, (float*)value);   return;
        case XNE_UNIFORM_DATA:
            // basic values bindings, we just memcpy everythings using the 'length' (= buffer size)
            glBindBuffer(GL_UNIFORM_BUFFER, shader->uniforms[index].location);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, shader->uniforms[index].length, value);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            return;

        case XNE_UNIFORM_LIGHT:
            {
                xne_Light_t* light = (xne_Light_t*)value;
                switch (light->type)
                {
                case XNE_LIGHT_DIRECTIONAL:
                    glBindBuffer(GL_UNIFORM_BUFFER, shader->uniforms[index].location);
                    glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(xne_vec4), sizeof(xne_vec3), &light->position[0]);
                    glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(xne_vec4), sizeof(xne_vec3), &light->light.directional.direction[0]);
                    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(xne_vec4), sizeof(xne_vec3), &light->light.directional.color[0]);
                    glBindBuffer(GL_UNIFORM_BUFFER, 0);
                    return;

                default:
                    xne_printf("cannot find light type!");
                    break;
                }
            }
            return;

        case XNE_UNIFORM_MATERIAL:
            {
                xne_Material_t* material = (xne_Material_t*)value;

                glBindBuffer(GL_UNIFORM_BUFFER, shader->uniforms[index].location);
                glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(xne_vec4), sizeof(xne_vec4), &material->ambient_color[0]);
                glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(xne_vec4), sizeof(xne_vec4), &material->diffuse_color[0]);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
            return;

        default:
            break;
        }
    }

    //xne_vprintf("failed to use uniform '%d' either the type is not avaible or the uniform is not defined!", index); 
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

    // release bindings block based uniforms
    for (size_t i = 0; i < shader->uniform_count; i++)
    {
        if((shader->uniforms[i].attrib & XNE_UNIFORM_ATTRIB_STRUCT)){
            
            glDeleteBuffers(1, &shader->uniforms[i].location);
        }
    }
    

    free(shader->shaders);
    glDeleteProgram(shader->program);
}