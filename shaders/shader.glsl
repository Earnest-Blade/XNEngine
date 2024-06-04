#version 400

// Start of the vertex shader
#ifdef _VERTEX_

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uvs;
layout(location = 2) in vec3 in_normals;

out V_DATA {
    vec2 uvs;
    vec3 normals;
    vec4 color;
} vertex;

uniform mat4 projection;
uniform mat4 transform;
uniform mat4 world;

void main() { 
    gl_Position = world * projection * transform * vec4(in_position, 1.0); 

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
    vec3 ambient_color = vec3(0.6, 0.6, 0.6);
    vec3 diffuse_color = vec3(0.0, 1.0, 0.6);
    vec3 light_color = vec3(1.0, 1.0, 1.0);

    vec3 light_pos = vec3(1.2, 1, 2.0);
    vec3 norm = normalize(vertex.normals);
    vec3 light_dir = normalize(light_pos - norm);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 frag = (ambient_color + diffuse) * diffuse_color;
    gl_FragColor = vec4(frag, 1.0); //texture(texture0, vertex.uvs); //vec4(vec3(gl_FragCoord.z), 1.0); //texture(texture0, vec3(uvs, 0));
}

#endif