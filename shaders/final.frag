#version 460 core

#include "perlin_noise.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform float time = 0.5; //value between 0 and 1

// in vec3 fragPos;
// in vec3 normal;
in vec2 texCoord;


out vec4 fragColor;

void main() {
    
    //fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);

    float noise = noise3D_to_1D(vec3(texCoord, time));
    //vec3 noise_vec = test_vec3(vec3(texCoord, time));
    vec2 noise_vec2 = noise3D_to_2D(vec3(texCoord, time));

    fragColor = vec4(noise_vec2, 0.0, 1.0);
    //fragColor = vec4(noise_vec, 1.0);
}