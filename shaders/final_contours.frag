#version 460 core

#include "contours.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec
uniform sampler2D normalTexture;
uniform int window_w;
uniform int window_h;

in vec2 texCoord;


out vec4 fragColor;

void main() {
    // vec2 newTexCoordl = texCoord;
    // vec2 newTexCoordr = texCoord;
    // float step = 0.005;
    // newTexCoordl.x = clamp(newTexCoordl.x - step, 0.0, 1.0);
    // newTexCoordr.x = clamp(newTexCoordr.x + step, 0.0, 1.0);
    // fragColor = vec4(texture(sceneTexture, newTexCoordl).b, 0.0, texture(sceneTexture, texCoord).b, 1.0);

    //fragColor = vec4(texture(sceneTexture, newTexCoord).rgb, 1.0);

    
    float contour_m = isContourFromMeshId(metadataTexture, texCoord, window_w, window_h);
    float contour_d = isContourFromDepth(normalTexture, texCoord, window_w, window_h);
    float contour_n = isContourFromNormal(normalTexture, texCoord, window_w, window_h);
    //vec3 normal = texture(normalTexture, texCoord).rgb;
    // if(normal == vec3(0.0, 0.0, 0.0)){
    //     fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    // }
    // else{
    //     normal = normal*2.0 - 1.0;
    //     fragColor = vec4(contour, contour, contour, 1.0);
        
    //     if(normal.x<0.0 || normal.y<0.0 || normal.z<0.0){
    //         fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //     }
    //     else{
    //         fragColor = vec4(0.0, 0.0, 1.0, 1.0);
    //     }
    // }
    
    // fragColor = vec4(normal.x, 0.0, 0.0, 1.0);
    //fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);
    fragColor = vec4(contour_m, contour_d, contour_n, 1.0);
    
}