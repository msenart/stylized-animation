#version 460 core

#define TOON_SHADING_DIFFUSE
#define TOON_SHADING_SPECULAR
#define TOON_SHADING_RIM_LIGHTING
#define TOON_SHADING_AMBIENT


#include "hybrid_metadata.glsl"
#include "light.glsl"
#include "toon_shading.glsl"

in vec3 normalO;
in vec3 localPosO;
in vec3 fragPos;
in vec3 debugColor; // DEBUG
flat in uint vertexID;

uniform uint meshId = 1;
uniform vec3 viewPos;
uniform uint activationBoneID = 7;
uniform uint lightsNumber;
uniform int totalVertices;
uniform int currentFrame;
uniform vec3 debug_c_r;
uniform vec3 debug_c_t;

// 2 output values : one for each color attachment
layout(location = 0) out vec4 FragColor0; //scene
layout(location = 1) out uvec4 FragColor1; //meshId

const uint MAX_NUM_BONES_PER_VERTEX = 16;

struct VertexBoneData {
    uint ids[MAX_NUM_BONES_PER_VERTEX];
    float weights[MAX_NUM_BONES_PER_VERTEX];
};

layout(std430, binding = 2) readonly buffer BoneBuffer {
    VertexBoneData allVertexBoneData[];
};
layout(std430, binding = 3) readonly buffer MotionOffsets { // NOTE do I have only to bind it once and it works here in the fragment shader as well?
    float deltas[];
};

uniform vec4 not_influenced_vertex_color = vec4(0.0, 0.0, 1.0, 1.0);
uniform vec4 influenced_vertex_color = vec4(1.0, 0.0, 0.0, 1.0);

vec3 ACESFilm(vec3 x) {
    float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

float distToLineSegment(vec3 p, vec3 a, vec3 b) {
    vec3 pa = p - a;
    vec3 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

void main() {
    uint deltaIdx = (currentFrame * totalVertices) + vertexID;
    float currentDelta = deltas[deltaIdx];

    //Write in color attachment 1 (MeshId)
    FragColor1 = getFragColor1(meshId);

    FragColor0 = vec4(0);
    // Bone highlighting
    float weight = 0.0;

    vec3 colorDeltaNeg = vec3(0.1, 0.1, 0.9);
    vec3 colorDeltaPos = vec3(0.9, 0.1, 0.1);
    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 deltaColorRed = mix(white, colorDeltaPos, currentDelta);
    vec3 deltaColorBlue = mix(white, colorDeltaNeg, currentDelta);
    vec3 deltaColor = mix(colorDeltaNeg, colorDeltaPos, currentDelta);

    FragColor0 = vec4(deltaColor, 1.0);
    // FragColor0 = vec4(debugColor, 1.0);
}
