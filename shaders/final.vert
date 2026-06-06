#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;


out vec3 fragPos;
out vec3 normal;

void main() {
    fragPos = aPos;
    normal = aNormal;
    gl_Position = vec4(aPos, 1.0);
}