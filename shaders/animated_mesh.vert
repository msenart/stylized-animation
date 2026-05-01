#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in uvec4 bonesIDs;
layout(location = 4) in vec4 weights;

out vec3 normalO;
out vec3 localPosO;
flat out uvec4 bonesIDsO;
out vec4 weightsO;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection*view*model*vec4(position,1.0);
    normalO = normal;
    bonesIDsO = bonesIDs;
    weightsO = weights;
    localPosO = position;
}
