#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;


// out vec3 fragPos;
// out vec3 normal;
out vec2 texCoord;

void main() {
    // fragPos = aPos;
    // normal = aNormal;
    texCoord = (aPos.xy+1)/2;
    gl_Position = vec4(aPos, 1.0);
}