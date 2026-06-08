#version 120

#include "constants.glsl"

float getPhongDiffuse(in vec3 L, in vec3 N) {
    return max(dot(N, L), 0.0);
}

float getPhongSpecular(in vec3 L, in vec3 N, in vec3 V, in float shininess) {
    vec3 R = reflect(-L, N);
    return pow(max(dot(R, V), 0.0), shininess);
}

float getGGXDiffuse(in vec3 L, in vec3 N) {
    return max(dot(N, L), 0.0) / PI;
}

float getGGXSpecular(in vec3 L, in vec3 N, in vec3 V, in float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    float D = a2 / (PI * pow(NdotH * NdotH * (a2 - 1.0) + 1.0, 2.0));

    float k = a / 2.0;
    float G1L = NdotL / (NdotL * (1.0 - k) + k);
    float G1V = NdotV / (NdotV * (1.0 - k) + k);
    float G = G1L * G1V;

    return (D * G) / max(4.0 * NdotL * NdotV, 0.001);
}