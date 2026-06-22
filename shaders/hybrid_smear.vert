#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform int currentFrame;
uniform int totalVertices; // or just calculate it some other way
uniform int totalFrames;
uniform int boneStride;
uniform float betaMax;

out vec3 normalO;
out vec3 localPosO;
out vec3 fragPos;
out vec3 debugColor; // DEBUG
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
    //ssbo (store buffer) that contains for each vertex
    //the bones it is associated with
    VertexBoneData allVertexBoneData[];
};
// WIP
layout(std430, binding = 3) readonly buffer MotionOffsets {
    float deltas[];
};
layout(std430, binding = 4) readonly buffer AnimationBones {
    mat4 allFramesBones[]; // this has offset applied already
};

int wrapFrame(int frame) {
    int r = frame % totalFrames;
    return r < 0 ? r + totalFrames : r;
}

vec3 getSkinnedPosition(int frameIdx) {
    int frame = wrapFrame(frameIdx);
    mat4 boneTransform = mat4(0.0f);
    VertexBoneData vertexBoneData = allVertexBoneData[gl_VertexID];

    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
        uint bone_id = vertexBoneData.ids[i];
        float weight = vertexBoneData.weights[i];
        if (weight > 0.0) {
            int flatIdx = (frame * boneStride) + int(bone_id);
            boneTransform += allFramesBones[flatIdx] * weight;
        }
    }
    return vec3(boneTransform * vec4(position, 1.0));
}

mat4 getSkinnedMatrix(int frameIdx) {
    int frame = wrapFrame(frameIdx);
    mat4 boneTransform = mat4(0.0f);
    VertexBoneData vertexBoneData = allVertexBoneData[gl_VertexID];

    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
        uint bone_id = vertexBoneData.ids[i];
        float weight = vertexBoneData.weights[i];
        int flatIdx = (frame * boneStride) + int(bone_id);
        boneTransform += allFramesBones[flatIdx] * weight;
    }
    return boneTransform;
}

vec3 catmullRom(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5 * (
        (2.0 * p1) +
        (-p0 + p2) * t +
        (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
        (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3
    );
}


void main() {
    int deltaIdx = (currentFrame * totalVertices) + gl_VertexID;
    float currentDelta = deltas[deltaIdx];
    vec3 posCurrent = getSkinnedPosition(currentFrame);
    vec3 posNext = getSkinnedPosition(currentFrame + 1);
    float speed = length(posNext - posCurrent);
    float beta = currentDelta * betaMax; // * 0.05 * speed;
    beta = clamp(beta, -betaMax, betaMax);
    int baseFrame = currentFrame + int(floor(beta));
    float t = beta - floor(beta);
    vec3 p0 = getSkinnedPosition(baseFrame - 1);
    vec3 p1 = getSkinnedPosition(baseFrame);
    vec3 p2 = getSkinnedPosition(baseFrame + 1);
    vec3 p3 = getSkinnedPosition(baseFrame + 2);

    vec3 smearedLocalPos = catmullRom(p0, p1, p2, p3, t);
    fragPos = vec3(model * vec4(smearedLocalPos, 1.0));
    gl_Position = projection * view * vec4(fragPos, 1.0);

    mat4 baseBoneTransform = getSkinnedMatrix(baseFrame);
    normalO = vec3(transpose(inverse(baseBoneTransform)) * vec4(normal, 0.0));

    localPosO = position;
    vertexID = gl_VertexID;

    float normBeta = (beta + 5.0) / 10.0;
    debugColor = vec3(1.0 - normBeta, normBeta, 0.0);
}
