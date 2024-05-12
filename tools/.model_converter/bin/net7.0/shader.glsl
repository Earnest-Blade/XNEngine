#version 400

// Start of the vertex shader
#ifdef _VERTEX_

attribute vec3 position;
attribute vec2 in_uvs;
attribute vec3 in_normals;

out V_DATA {
    vec2 uvs;
    vec3 normals;
    vec4 color;
} vertex;

uniform mat4 projection;
uniform mat4 transform;

void main() { 
    gl_Position = projection * transform * vec4(position, 1.0); 

    vertex.uvs = in_uvs;
    vertex.normals = in_normals;
    vertex.color = vec4(1.0, 1.0, 1.0, 1.0);
}

#endif

#ifdef _GEOMETRY_

layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

in V_DATA {
    vec2 uvs;
    vec3 normals;
    vec4 color;
} vertices[];

out F_DATA {
    vec2 uvs;
    vec3 normals;
    vec4 color;
} frag;

void main(){
    int i;
    for(i = 0; i < gl_in.length(); i++){
        frag.uvs = vertices[i].uvs;
        frag.normals = vertices[i].normals;
        frag.color = vertices[i].color;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}

#endif

// Start of the fragment shader
#ifdef _FRAGMENT_

in V_DATA {
    vec2 uvs;
    vec3 normals;
    vec4 color;
} vertex;

uniform int layer;
uniform sampler2D texture0;

void main() { 
    gl_FragColor = texture(texture0, vertex.uvs); //texture(texture0, vec3(uvs, 0));
}

#endif