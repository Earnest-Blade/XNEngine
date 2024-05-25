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
    vec2 pixels_size = vec2(1920 / 5, 1080 / 5);
    vec2 p = uvs;
    p.x -= mod(p.x, 1.0 / pixels_size.x);
    p.y -= mod(p.y, 1.0 / pixels_size.y);

    vec3 frame = texture2D(frame0, p).rgb;
    gl_FragColor = vec4(frame, 1.0);
}

#endif