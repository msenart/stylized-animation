#version 460 core

#include "contours.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec
uniform sampler2D normalTexture; //contains normals and z value (in alpha component)
uniform int window_w;
uniform int window_h;

in vec2 texCoord;


out vec4 fragColor;

void main() {

    float contour = isContourFromDepth(normalTexture, texCoord, window_w, window_h);
    
    fragColor = vec4(vec3(contour), 1.0);
    

    
}