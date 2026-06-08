#version 460 core

#include "perlin_noise.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec
uniform float time = 0.5; //value between 0 and 1

// in vec3 fragPos;
// in vec3 normal;
in vec2 texCoord;


out vec4 fragColor;

void main() {

    //vec2 noise_vec2 = noise3D_to_2D(vec3(texCoord, time));
    //fragColor = vec4(noise_vec2, 0.0, 1.0);
    
    fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);
}