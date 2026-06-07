#version 460 core

#include "perlin_noise.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform float t = 0.5; //time

// in vec3 fragPos;
// in vec3 normal;
in vec2 texCoord;


out vec4 fragColor;

void main() {
    
    //fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);

    float noise = noise3D(vec3(texCoord, t));

    fragColor = vec4(vec3(noise), 1.0);
    //fragColor = vec4(noise_vec, 1.0);
}