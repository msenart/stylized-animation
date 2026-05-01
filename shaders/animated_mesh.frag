#version 460 core

in vec3 normalO;
in vec3 localPosO;
flat in uvec4 bonesIDsO;
in vec4 weightsO;

uniform uint activationBoneID = 1;

out vec4 FragColor;

uniform vec4 not_influenced_vertex_color = vec4(0.0, 0.0, 1.0, 1.0);
uniform vec4 influenced_vertex_color = vec4(1.0, 0.0, 0.0, 1.0);

void main() {
    float weight = 0.0;

    if (bonesIDsO.x == activationBoneID) {
        weight = weightsO.x;
    } else if (bonesIDsO.y == activationBoneID) {
        weight = weightsO.y;
    } else if (bonesIDsO.z == activationBoneID) {
        weight = weightsO.z;
    } else if (bonesIDsO.w == activationBoneID) {
        weight = weightsO.w;
    }

    FragColor = mix(not_influenced_vertex_color, influenced_vertex_color, weight);
}