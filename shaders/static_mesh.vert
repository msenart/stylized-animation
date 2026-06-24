#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 fragNorm;
out vec2 uvO; // default shader output : (0,0)

void main() {
    gl_Position = projection*view*model*vec4(position,1.0);
    uvO = uv;
    fragPos = position;
}
