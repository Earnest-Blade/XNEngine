#pragma once

#ifndef XNE_SHADER_IMPL
#define XNE_SHADER_IMPL

#define XNE_CORE_FILE
#define XNE_CORE_STRING
#include "core/core.h"

#include <stdio.h>

typedef enum xne_UniformAttrib {
    XNE_UNIFORM_ATTRIB_UNIFORM = 1,
} xne_UniformAttrib_t;

typedef enum xne_UniformType {
    XNE_UNIFORM_FLOAT,
    XNE_UNIFORM_INT,
    XNE_UNIFORM_VEC2,
    XNE_UNIFORM_VEC3,
    XNE_UNIFORM_VEC4,
    XNE_UNIFORM_MAT4
} xne_UniformType_t;

typedef enum xne_ShaderType {
    XNE_VERTEX_SHADER = 0x8B31,
    XNE_FRAGMENT_SHADER = 0x8B30,
    XNE_GEOMETRY_SHADER = 0x8DD9
} xne_ShaderType_t;

#define XNE_SHADER_END() 0, ""
typedef struct xne_ShaderDesc {
    int type;
    const char* name;
} xne_ShaderDesc_t;

#define XNE_SHADER_UNIFORM_END() 0, XNE_UNIFORM_FLOAT, ""
typedef struct xne_ShaderUniformDesc {
    int attrib;
    xne_UniformType_t format;
    const char* name;
} xne_ShaderUniformDesc_t;

typedef struct xne_Shader {
    struct xne_ShaderUniform {
        xne_Enum_t attrib;
        xne_Enum_t format;
        uint32_t location;   
    } *uniforms;
    uint32_t* shaders;
    uint32_t program;
    size_t count, uniform_count;
} xne_Shader_t;

/*
    Initialize a new shader struct by reading from a file.
    @param shader Pointer to the shader to initialize.
    @param file File to read from.
    @param desc Array of the sub shaders.
    @return return XNE_OK or XNE_FAILURE if the shader have been succesfully created.
*/
int xne_create_shaderf(xne_Shader_t* shader, FILE* file, const xne_ShaderDesc_t* desc);

/*
    Initialize a new shader struct by reading from a relative or absolute path.
    @param shader Pointer to the shader to initialize.
    @param path Path to the file to read from.
    @param desc Array of the sub shaders.
    @return return XNE_OK or XNE_FAILURE if the shader have been succesfully created.
*/
static inline int xne_create_shader(xne_Shader_t* shader, const char* path, const xne_ShaderDesc_t* desc){
    FILE* f = fopen(path, "rb");
    int s = xne_create_shaderf(shader, f, desc);
    fclose(f);
    return s;
}

/*
    Initialize a new shader struct from vertex and fragment shader as strings.
    @param shader Pointer to the shader to initialize.
    @param vert Vertex Shader value as string.
    @param frag Fragment Shader value as string.
    @return return XNE_OK or XNE_FAILURE if the shader have been succesfully created.
*/
int xne_create_shaderfv(xne_Shader_t* shader, const char* vert, const char* frag);

/*
    Prepare a shader to be use for rendering.
    @param shader Pointer to the shader.
*/
void xne_shader_enable(xne_Shader_t* shader);

void xne_shader_disable(xne_Shader_t* shader);

/*
    Free a shader struct.
    @param shader Pointer to the shader.
*/
void xne_destroy_shader(xne_Shader_t* shader);

/*
    Return if a shader is sucessfully compiled, linked and if it can be use.
    @param shader Pointer to the shader.
*/
int xne_shader_is_valid(xne_Shader_t* shader);

/*
    Return a pointer to the active shader.
*/
xne_Shader_t* xne_get_active_shader();

int xne_link_shader_uniforms(xne_Shader_t* shader, const xne_ShaderUniformDesc_t* uniforms);

void xne_shader_use_uniform(xne_Shader_t* shader, uint32_t index, const void* value);

#endif