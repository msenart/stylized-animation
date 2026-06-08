#version 460 core


uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec

in vec2 texCoord;


out vec4 fragColor;

void main() {
    fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);
}