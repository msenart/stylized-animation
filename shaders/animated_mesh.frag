#version 460 core

#define TOON_SHADING_DIFFUSE
//#define TOON_SHADING_SPECULAR
#define TOON_SHADING_RIM_LIGHTING

in vec3 normalO;
in vec3 localPosO;
in vec3 fragPos;
flat in uint vertexID;

uniform vec3 viewPos;
uniform uint activationBoneID = 7;
const float PI = 3.14159265358979323846;

out vec4 FragColor;

const uint MAX_NUM_BONES_PER_VERTEX = 16;

struct VertexBoneData {
    uint ids[MAX_NUM_BONES_PER_VERTEX];
    float weights[MAX_NUM_BONES_PER_VERTEX];
};

layout(std430, binding = 2) readonly buffer BoneBuffer {
    VertexBoneData allVertexBoneData[];
};

#include "light.glsl"

uniform vec4 not_influenced_vertex_color = vec4(0.0, 0.0, 1.0, 1.0);
uniform vec4 influenced_vertex_color = vec4(1.0, 0.0, 0.0, 1.0);

float exp_smoothstep(in float x, in float speed){
    return x < 0 ? exp(speed*x)/(1+exp(speed*x)) : 1/(1+exp(-speed*x));
}

float sin_smoothstep(in float x, in float speed){
    if (x < -PI/(2*speed)){
        return 0;
    }
    else if (x > PI/(2*speed)) {
        return 1;
    }
    else{
        return (sin(speed*x) + 1)/2.f;
    }

}

void main() {
    FragColor = vec4(1);
    // Bone highlighting
    float weight = 0.0;
    VertexBoneData vertex_bone_data = allVertexBoneData[vertexID];
    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
        if (vertex_bone_data.ids[i] == activationBoneID) {
            weight = vertex_bone_data.weights[i];
            break;
        }
    }
    FragColor *= mix(not_influenced_vertex_color, influenced_vertex_color, weight);

    // Toon shading diffuse implementation
    vec3 N = normalize(normalO);
    vec3 L = normalize(allLights[0].position-fragPos); // To change !
    float k_ambient = 0.3;
    float NdotL = dot(N, L);
    float k_diffuse = max(NdotL, 0.0);
    vec3 V = normalize(viewPos - fragPos);
    vec3 R = normalize(reflect(L,N));

    #ifdef TOON_SHADING_DIFFUSE
    FragColor.rgb *= k_ambient + (1-k_ambient)*sin_smoothstep(NdotL, 5);
    #else
    FragColor.rgb *= max(k_diffuse, 0.3);
    #endif

    #ifdef TOON_SHADING_SPECULAR
    FragColor.rgb += vec3(1)*pow(sin_smoothstep(dot(-V,R),50),5);
    #endif

    #ifdef TOON_SHADING_RIM_LIGHTING
    float rimDot = 1 - sin_smoothstep(dot(V, N),5);
        FragColor.rgb += vec3(1)*rimDot*k_diffuse;
    #endif

    #ifdef CONTOURS
    float epsilon = 0.3;
    vec3 V = normalize(viewPos - fragPos);

    if (max(dot(N, V), 0.0) < epsilon) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    #endif
}