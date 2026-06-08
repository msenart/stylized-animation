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
flat in uint vertexID;

uniform vec3 viewPos;
uniform uint activationBoneID = 7;
uniform uint lightsNumber;

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

uniform vec4 not_influenced_vertex_color = vec4(0.0, 0.0, 1.0, 1.0);
uniform vec4 influenced_vertex_color = vec4(1.0, 0.0, 0.0, 1.0);

vec3 ACESFilm(vec3 x) {
    float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main() {

    //Write in color attachment 1 (MeshId)
    FragColor1 = getFragColor1(1);

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
    float k_ambient = 0.05f;
    float k_diffuse = 1.0f;
    float k_specular = 0.3f;
    float k_rim = 0.5f;
    #ifdef TOON_SHADING_AMBIENT
    FragColor0.rgb += baseColor*k_ambient;
    #endif

    // Toon shading diffuse implementation
    for (int i= 0; i < lightsNumber; i++){
        Light light = allLights[i];
        vec3 L = normalize(light.position-fragPos); // To change !
        vec3 R = normalize(reflect(L,N));

        #ifdef TOON_SHADING_DIFFUSE
//        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*sin_smoothstep(dot(N, L), 0.5f);
//        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*sin_smoothstep(dot(N, L), -0.3f, 0.3f);
//        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*halftone(gl_FragCoord.xy,0.1f, sin_smoothstep(NdotL, 0.5f));
        float NdotL = dot(N, L);
        float shadow_to_midtone = sin_smoothstep(NdotL,-0.2f, 0.05f)*crosshatching(gl_FragCoord.xy,0.1f,mix(NdotL,0.0f, 0.1f));
        float midtone_to_light = sin_smoothstep(NdotL,0.35f, 0.5f);
        FragColor0.rgb += k_diffuse*light.color*baseColor*(1-k_ambient)*(max(shadow_to_midtone,midtone_to_light));
        #endif

        #ifdef TOON_SHADING_SPECULAR
        FragColor0.rgb += k_specular*light.color*halftone(gl_FragCoord.xy,0.15f,sin_smoothstep(2*pow(max(dot(V,-R),0)*max(dot(N,L),0),2)-1,-1.0f,1.0f));
//        FragColor0.rgb += light.color*pow(sin_smoothstep(dot(-V,R),2),0.5f);
        #endif

        #ifdef TOON_SHADING_RIM_LIGHTING
        float rimDot = 1 - dot(V, N);
        FragColor0.rgb += k_rim*light.color*k_ambient*(1-sin_smoothstep(1-rimDot*pow(max(dot(N,L),0.001f),0.1f),0.0f, 0.5f));
        #endif
    }

    #ifdef CONTOURS
    float epsilon = 0.3;

    if (max(dot(N, V), 0.0) < epsilon) {
        FragColor0.rgb *= 0;
    }
    #endif

    // naive HDR processing
    FragColor0.rgb = ACESFilm(FragColor0.rgb);


//    FragColor0 = vec4(halftone(gl_FragCoord.xy,0.05f, 0.5f));
    FragColor0 = vec4(pow(FragColor0.rgb, vec3(1.0/2.2)), 1.0);
}