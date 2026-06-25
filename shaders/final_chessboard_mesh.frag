#version 460 core
#include "bsdf_utilities.glsl"
#include "hybrid_metadata.glsl"
#include "light.glsl"

in vec2 uvO;

const float chessboard_scale = 10.f;

layout(location = 0) out vec4 FragColor0;
layout(location = 1) out uvec4 FragColor1;

uniform uint meshId;
uniform vec3 fragPos;
uniform vec3 viewPos;
uniform vec3 fragNorm;

float chessboard(vec2 uv) {
    vec2 p = floor(uv);
    return mod(p.x + p.y, 2.0);
}

// making the chessboard
void main() {
    // fill the mesh id buffer

    FragColor1 = getFragColor1(meshId);

    // fill the render buffer
    float isWhite = chessboard(uvO*10);
    if (isWhite == 1.f){
        FragColor0 = vec4(1.0);
        FragColor0.xyz = vec3(0.8);
    }
    else{
        FragColor0 = vec4(1.0);
        FragColor0.xyz = vec3(0.2);
    }
    vec3 N = fragNorm;
    vec3 V = normalize(viewPos-fragPos);

    for (int i = 0; i < lightsNumber; i++){
        Light light = allLights[i];
        vec3 L = normalize(light.position-fragPos);

        FragColor0.xyz += light.color*getPhongDiffuse(L,N);
        FragColor0.xyz += light.color*getPhongSpecular(L,N,V,20);
    }
    FragColor0.xyz = ACESFilm(FragColor0.xyz);
}
