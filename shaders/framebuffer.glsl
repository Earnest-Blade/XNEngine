#version 400

#ifdef _VERTEX_

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uvs;

out vec2 uvs;

void main(){
    gl_Position = vec4(in_position.x, in_position.y, 0.0f, 1.0f);
    uvs = in_uvs;
}

#endif

#ifdef _FRAGMENT_

in vec2 uvs;

uniform sampler2D frame0;

void main(){
    gl_FragColor = texture(frame0, uvs);
}

#endif