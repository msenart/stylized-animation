#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 normalO;
out vec3 localPosO;
flat out uint vertexID;

const uint MAX_NUM_BONES_PER_VERTEX = 16;
const uint MAX_NUM_BONES = 128;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_NUM_BONES];

struct VertexBoneData {
    uint ids[MAX_NUM_BONES_PER_VERTEX];
    float weights[MAX_NUM_BONES_PER_VERTEX];
};

layout(std430, binding = 2) readonly buffer BoneBuffer {
    VertexBoneData allVertexBoneData[];
};

void main() {
    mat4 boneTransform = mat4(0.0f);
    VertexBoneData vertexBoneData = allVertexBoneData[gl_VertexID];
    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++){
        uint bone_id = vertexBoneData.ids[i];
        float weight = vertexBoneData.weights[i];
        boneTransform+=gBones[bone_id]*weight;
    }
    vec4 localPos = boneTransform*vec4(position,1.0);
    gl_Position = projection*view*model*localPos;
    normalO = normal;
    localPosO = position;
    vertexID = gl_VertexID;
}
