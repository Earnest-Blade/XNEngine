#version 400

// Start of the vertex shader
#ifdef _VERTEX_

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uvs;

out V_DATA {
    vec2 uvs;
} vertex;

uniform vec3 eye;

uniform mat4 projection;
uniform mat4 transform;

void main() { 
    gl_Position = projection * transform  * vec4(in_position, 1.0); 

    vertex.uvs = in_uvs;
}

#endif

// Start of the fragment shader
#ifdef _FRAGMENT_

in V_DATA {
    vec2 uvs;
} vertex;

uniform sampler2DArray texture0;

void main() { 
    int row = 4;
    int layer = 0;
    int face = 1;

    vec4 frag = texture(texture0, vec3(vertex.uvs, layer * row + face));
    if(frag.a < 0.1){
        discard;
    }
    
    gl_FragColor = vec4(vec3(frag), 2);
}

#endif