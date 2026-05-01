#version 460 core

in vec3 normalO;
in vec3 localPosO;
flat in uint vertexID;

uniform uint activationBoneID = 13;

out vec4 FragColor;

const uint MAX_NUM_BONES_PER_VERTEX = 16;

struct VertexBoneData {
    uint ids[MAX_NUM_BONES_PER_VERTEX];
    float weights[MAX_NUM_BONES_PER_VERTEX];
};

layout(std430, binding = 2) readonly buffer BoneBuffer {
    VertexBoneData allBones[];
};

uniform vec4 not_influenced_vertex_color = vec4(0.0, 0.0, 1.0, 1.0);
uniform vec4 influenced_vertex_color = vec4(1.0, 0.0, 0.0, 1.0);

void main() {
    float weight = 0.0;
    VertexBoneData vertex_bone_data = allBones[vertexID];
    for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++){
        if (vertex_bone_data.ids[i] == activationBoneID){
            weight = vertex_bone_data.weights[i];
            break;
        }
    }

    FragColor = mix(not_influenced_vertex_color, influenced_vertex_color, weight);
    FragColor *= max(dot(normalO,vec3(1,1,1)),0.3);
}