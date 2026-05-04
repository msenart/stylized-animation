#version 460 core

//#define TOON_SHADING

in vec3 normalO;
in vec3 localPosO;
flat in uint vertexID;

uniform uint activationBoneID =7;

out vec4 FragColor;

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

void main() {
    float weight = 0.0;
    VertexBoneData vertex_bone_data = allVertexBoneData[vertexID];
    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++){
        if (vertex_bone_data.ids[i] == activationBoneID){
            weight = vertex_bone_data.weights[i];
            break;
        }
    }

    FragColor = mix(not_influenced_vertex_color, influenced_vertex_color, weight);
    float diffuse_coeff = dot(normalO,vec3(1,1,1));
    
    #ifdef TOON_SHADING 
    float diffuse_threshold = 0.4;
    if(diffuse_coeff < diffuse_threshold){
        FragColor *= 0.3;
    }
    #else
    FragColor *= max(diffuse_coeff, 0.3);
    #endif
    
}