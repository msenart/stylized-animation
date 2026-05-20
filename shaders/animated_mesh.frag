#version 460 core

in vec3 normalO;
in vec3 localPosO;
in vec3 fragPos;
flat in uint vertexID;

uniform vec3 viewPos;
uniform uint activationBoneID = 7;

out vec4 FragColor;

const uint MAX_NUM_BONES_PER_VERTEX = 16;

struct VertexBoneData {
    uint ids[MAX_NUM_BONES_PER_VERTEX];
    float weights[MAX_NUM_BONES_PER_VERTEX];
};

layout(std430, binding = 2) readonly buffer BoneBuffer {
    VertexBoneData allVertexBoneData[];
};

uniform vec4 not_influenced_vertex_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 influenced_vertex_color = vec4(1.0, 0.0, 0.0, 1.0);

void main() {
    float weight = 0.0;

    VertexBoneData vertex_bone_data = allVertexBoneData[vertexID];
    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
        if (vertex_bone_data.ids[i] == activationBoneID) {
            weight = vertex_bone_data.weights[i];
            break;
        }
    }

    FragColor = mix(not_influenced_vertex_color, influenced_vertex_color, weight);

    vec3 N = normalize(normalO);
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    float diffuse_coeff = max(dot(N, L), 0.0);

    #ifdef TOON_SHADING
    float diffuse_threshold = 0.4;
    if(diffuse_coeff < diffuse_threshold) {
        FragColor.rgb *= 0.3;
    }
    #else
    FragColor.rgb *= max(diffuse_coeff, 0.3);
    #endif

    #ifdef CONTOURS
    float epsilon = 0.3;
    vec3 V = normalize(viewPos - fragPos);

    if (max(dot(N, V), 0.0) < epsilon) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    #endif
}