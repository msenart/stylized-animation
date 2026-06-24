#version 460 core

struct Light {
    vec3 position;
    float pad1;
    vec3 color;
    float pad2;
    float intensity;
};

uniform uint lightsNumber;

layout(std430, binding = 1) readonly buffer LightBuffer {
    Light allLights[];
};
