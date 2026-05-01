#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 normalO;
out vec3 localPosO;
flat out uint vertexID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const uint MAX_NUM_BONES_PER_VERTEX = 16;

struct VertexBoneData {
    uint ids[MAX_NUM_BONES_PER_VERTEX];
    float weights[MAX_NUM_BONES_PER_VERTEX];
};

layout(std430, binding = 2) readonly buffer BoneBuffer {
    VertexBoneData allBones[];
};

void main() {
    gl_Position = projection*view*model*vec4(position,1.0);
    normalO = normal;
    localPosO = position;
    vertexID = gl_VertexID;
}
