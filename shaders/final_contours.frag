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

    
    float contour_m = isContourFromMeshId(metadataTexture, texCoord, window_w, window_h);
    float contour_d = isContourFromDepth(normalTexture, texCoord, window_w, window_h);
    float contour_n = isContourFromNormal(normalTexture, texCoord, window_w, window_h);
    vec3 normal = texture(normalTexture, texCoord).rgb;
    if(normal == vec3(0.0, 0.0, 0.0)){
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        //normal = normal*2.0 - 1.0;
        
        if(normal.x<0.0 || normal.y<0.0 || normal.z<0.0){
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else{
            fragColor = vec4(0.0, 0.0, 1.0, 1.0);
        }
    }
    
    // fragColor = vec4(normal.x, 0.0, 0.0, 1.0);
    //fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);
    //fragColor = vec4(contour_m, contour_d, contour_n, 1.0);
    fragColor = vec4(0.0, contour_d, 0.0, 1.0);
    //float depth = testDepth(normalTexture, texCoord, window_w, window_h);
    //fragColor = vec4(depth, depth, depth, 1.0);

    // float depth = texture(normalTexture, texCoord).a;
    // if(depth == 0.0 ){
    //     fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    // }
    // else if(depth<-5.0){
    //     fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // }
    // else{
    //     fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // }

    
}