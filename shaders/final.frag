#version 460 core


uniform sampler2D sceneTexture; //the scene has been renderered in this texture

in vec3 fragPos;
in vec3 normal;


out vec4 fragColor;

void main() {
    
    vec2 vTex = (fragPos.xy+1)/2;
    fragColor = vec4(texture(sceneTexture, vTex).rgb, 1.0);
}