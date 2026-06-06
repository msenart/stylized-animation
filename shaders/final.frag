#version 460 core


uniform sampler2D sceneTexture; //the scene has been renderered in this texture

// in vec3 fragPos;
// in vec3 normal;
in vec2 texCoord;


out vec4 fragColor;

void main() {
    
    //vec2 vTex = (fragPos.xy+1)/2;
    fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);
}