#include "framebuffer.h"

#include <assert.h>
#include <stdio.h>

#include <GL/glew.h>


void xne_create_framebuffer(xne_Framebuffer_t* framebuffer, uint32_t width, uint32_t height){
    assert(framebuffer);
    assert(width > 0 && height > 0);

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->prev_width = width;
    framebuffer->prev_height = height;

    {
        const float quad[24] = {
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        glGenVertexArrays(1, &framebuffer->vertex_object);
        glBindVertexArray(framebuffer->vertex_object);

        glGenBuffers(1, &framebuffer->array_object);
        glBindBuffer(GL_ARRAY_BUFFER, framebuffer->array_object);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), quad, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    const char* vertex = ""                                                   
        "#version 400\n"
        "layout(location = 0) in vec2 in_position;\n"                                   
        "layout(location = 1) in vec2 in_uvs;\n"                                       
        "out vec2 uvs;\n"                                                               
        "void main(){\n"                                        
        "    gl_Position = vec4(in_position.x, in_position.y, 0.0f, 1.0f);\n"          
        "    uvs = in_uvs;\n"                                                           
        "}\n"
    ;                                          
    const char* fragment = ""                                                       
        "#version 400\n"                                                        
        "in vec2 uvs;\n"                                                             
        "uniform sampler2D frame0;\n"                                                        
        "void main(){\n"                                                                
        "    gl_FragColor = texture(frame0, uvs);\n"                                    
        "}\n"                                                                           
    ;

    xne_create_shaderfv(&framebuffer->shader, vertex, fragment);

    glGenFramebuffers(1, &framebuffer->buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->buffer);

    glGenTextures(1, &framebuffer->color_buffer);
    glBindTexture(GL_TEXTURE_2D, framebuffer->color_buffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->color_buffer, 0);

    glGenRenderbuffers(1, &framebuffer->depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depth_buffer);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void xne_framebuffer_enable(xne_Framebuffer_t* framebuffer){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->buffer);

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    framebuffer->prev_width = viewport[2];
    framebuffer->prev_height = viewport[3];

    glViewport(0, 0, framebuffer->width, framebuffer->height);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void xne_framebuffer_disable(xne_Framebuffer_t* framebuffer){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
}

void xne_clear_framebuffer(xne_Framebuffer_t* framebuffer, float r, float g, float b){
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void xne_blit_framebuffer(xne_Framebuffer_t* framebuffer){
    xne_shader_enable(&framebuffer->shader);

    glBindVertexArray(framebuffer->vertex_object);
    glBindTexture(GL_TEXTURE_2D, framebuffer->color_buffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    xne_shader_disable(NULL);
}

void xne_destroy_framebuffer(xne_Framebuffer_t* framebuffer){
    xne_destroy_shader(&framebuffer->shader);

    glDeleteVertexArrays(1, &framebuffer->vertex_object);
    glDeleteBuffers(1, &framebuffer->array_object);
    glDeleteTextures(1, &framebuffer->color_buffer);
    glDeleteRenderbuffers(1, &framebuffer->depth_buffer);
    glDeleteFramebuffers(1, &framebuffer->buffer);
}