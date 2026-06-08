#version 460 core

#define TOON_SHADING_DIFFUSE
#define TOON_SHADING_SPECULAR
#define TOON_SHADING_RIM_LIGHTING
#define TOON_SHADING_AMBIENT

#include "hybrid_metadata.glsl"

in vec3 normalO;
in vec3 localPosO;
in vec3 fragPos;
flat in uint vertexID;

uniform vec3 viewPos;
uniform uint activationBoneID = 7;
const float PI = 3.14159265358979323846;

// 2 output values : one for each color attachment
layout(location = 0) out vec4 FragColor0; //scene
layout(location = 1) out uvec4 FragColor1; //meshId

const uint MAX_NUM_BONES_PER_VERTEX = 16;
const uint MAX_LIGHTS = 1;

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

vec3 ACESFilm(vec3 x) {
    float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main() {

    //Write in color attachment 1 (MeshId)
    FragColor1 = getFragColor1(1);

    //write in color attachment 2 (scene)
    FragColor0 = vec4(0);
    // Bone highlighting
    float weight = 0.0;
    VertexBoneData vertex_bone_data = allVertexBoneData[vertexID];
    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
        if (vertex_bone_data.ids[i] == activationBoneID) {
            weight = vertex_bone_data.weights[i];
            break;
        }
    }
    vec3 baseColor = mix(not_influenced_vertex_color, influenced_vertex_color, weight).rgb;
    vec3 N = normalize(normalO);
    vec3 V = normalize(viewPos - fragPos);
    float k_ambient = 0.3;
    // Toon shading diffuse implementation
    for (int i= 0; i < MAX_LIGHTS; i++){
        Light light = allLights[i];
        vec3 L = normalize(light.position-fragPos); // To change !
        float NdotL = dot(N, L);
        float k_diffuse = max(NdotL, 0.0);
        vec3 R = normalize(reflect(L,N));

        #ifdef TOON_SHADING_AMBIENT
        FragColor0.rgb += baseColor*k_ambient;
        #endif

        #ifdef TOON_SHADING_DIFFUSE
        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*sin_smoothstep(NdotL, 5);
        #endif

        #ifdef TOON_SHADING_SPECULAR
        FragColor0.rgb += light.color*pow(sin_smoothstep(dot(-V,R),2),50);
        #endif

        #ifdef TOON_SHADING_RIM_LIGHTING
        float rimDot = 1 - sin_smoothstep(dot(V, N),5);
        FragColor0.rgb += light.color*rimDot*k_diffuse;
        #endif

        #ifdef CONTOURS
        float epsilon = 0.3;
        vec3 V = normalize(viewPos - fragPos);

        if (max(dot(N, V), 0.0) < epsilon) {
            FragColor0.rgb *= 0;
        }
        #endif

        // naive HDR processing
        FragColor0.rgb = ACESFilm(FragColor0.rgb);

        FragColor0 = vec4(pow(FragColor0.rgb, vec3(1.0/2.2)), 1.0);
    }

}