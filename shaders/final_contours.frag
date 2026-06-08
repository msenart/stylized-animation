#version 460 core

#include "contours.glsl"
#include "perlin_noise.glsl"

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

    
    //float contour_m = isContourFromMeshId(metadataTexture, texCoord, window_w, window_h);
    float contour_d = isContourFromDepth(normalTexture, texCoord, window_w, window_h);
    //float contour_n = isContourFromNormal(normalTexture, texCoord, window_w, window_h);
    
    //fragColor = vec4(contour_m, contour_d, contour_n, 1.0);
    fragColor = vec4(contour_d, contour_d, contour_d, 1.0);
    

    
}