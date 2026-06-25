#version 460 core

#define TOON_SHADING_DIFFUSE
#define TOON_SHADING_SPECULAR
#define TOON_SHADING_RIM_LIGHTING
#define TOON_SHADING_AMBIENT
//#define GGX_BSDF

#include "hybrid_metadata.glsl"
#include "light.glsl"
#include "toon_shading_utilities.glsl"
#include "bsdf_utilities.glsl"

in vec3 normalO;
in vec3 localPosO;
in vec3 fragPos;
in vec2 uvO;
in float realZ;
flat in uint vertexID;

uniform uint meshId;
uniform vec3 viewPos;
uniform uint activationBoneID = 7;
//uniform vec3 objectBarycentre;
uniform mat4 view;
uniform mat4 projection;

// 2 output values : one for each color attachment
layout(location = 0) out vec4 FragColor0; //scene
layout(location = 1) out uvec4 FragColor1; //meshId
layout(location = 2) out vec4 FragColor2; //normal buffer

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
uniform int       hasTexture   = 0;
uniform sampler2D albedoTexture;

void main() {

    //Write in color attachment 1 (MeshId)
    FragColor1 = getFragColor1(meshId);

    //write in normal buffer
    FragColor2 = getFragColor2(normalO, realZ);

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
    vec3 baseColor = (hasTexture == 1)
        ? texture(albedoTexture, uvO).rgb
        : mix(not_influenced_vertex_color, influenced_vertex_color, weight).rgb;
    vec3 N = normalize(normalO);
    vec3 V = normalize(viewPos - fragPos);
    float k_ambient = 0.2f;
    float k_diffuse = 1.0f;
    float k_specular = 1.0f;
    float k_rim = 0.5f;
    #ifdef TOON_SHADING_AMBIENT
    FragColor0.rgb += baseColor*k_ambient;
    #endif

    // Toon shading diffuse implementation
    for (int i= 0; i < lightsNumber; i++){
        Light light = allLights[i];
        light.color = vec3(1); // <--------------------------------- shouldn't be here ! has to be removed !
        vec3 L = normalize(light.position-fragPos);
        vec3 R = normalize(reflect(L,N));

        vec2 patternPos = gl_FragCoord.xy;

        float diffuse = 0;
        float specular = 0;
        #ifdef GGX_BSDF
        float roughness = 0.6f;
        diffuse = getGGXDiffuse(L,N);
        specular = getGGXSpecular(L,N,V,roughness);
        #endif
        #ifndef GGX_BSDF
        float shininess = 2.f;
        diffuse = 1.2f*getPhongDiffuse(L,N);
        specular = getPhongSpecular(L,N,V,shininess);
        #endif

        #ifdef TOON_SHADING_DIFFUSE
//        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*sin_smoothstep(dot(N, L), 0.5f);
//        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*sin_smoothstep(dot(N, L), -0.3f, 0.3f);
//        FragColor0.rgb += light.color*baseColor*(1-k_ambient)*halftone(gl_FragCoord.xy,0.1f, sin_smoothstep(NdotL, 0.5f));
        float shadow_to_midtone = sin_smoothstep(diffuse,-0.2f, 0.05f)*crosshatching(patternPos,0.1f,mix(diffuse,0.0f, 0.1f));
        float midtone_to_light = sin_smoothstep(diffuse,0.35f, 0.5f);
        FragColor0.rgb += k_diffuse*light.color*baseColor*(1-k_ambient)*(max(shadow_to_midtone,midtone_to_light));
        #endif
        #ifndef TOON_SHADING_DIFFUSE
        FragColor0.rgb += k_diffuse*light.color*baseColor*diffuse;
        #endif

        #ifdef TOON_SHADING_SPECULAR
        FragColor0.rgb += k_specular*light.color*halftone(patternPos,0.15f,sin_smoothstep(2*pow(specular,2)-1,-1.0f,1.0f));
//        FragColor0.rgb += light.color*pow(sin_smoothstep(dot(-V,R),2),0.5f);
        #endif
        #ifndef TOON_SHADING_SPECULAR
        FragColor0.rgb += k_specular*light.color*specular;
        #endif

        #ifdef TOON_SHADING_RIM_LIGHTING
        float rimDot = 1 - dot(V, N);
        FragColor0.rgb += k_rim*light.color*k_ambient*(1-sin_smoothstep(1-rimDot*pow(diffuse,0.1f),0.0f, 0.5f));
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
    FragColor0 = vec4(pow(FragColor0.rgb, vec3(1.0/2.2)), 1.0);
}
