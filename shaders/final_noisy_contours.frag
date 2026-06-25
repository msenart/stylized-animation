#version 460 core

#include "contours.glsl"
#include "perlin_noise.glsl"

//texture
uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec
uniform sampler2D normalTexture; //contains normals and z value (in alpha component)

//window (for contour detection)
uniform int window_w;
uniform int window_h;

//noise
//for far view : 0.02, 50., 20.;
//vener : 0.03, 30.0, 20.0
//slower : 0.02, 20.0, 15.0
uniform float deformation = 0.02; 
uniform float space_noise_scale = 50.0; 
uniform float time_noise_scale = 20.0; 
uniform float time; //value between 0 and 1

in vec2 texCoord;


out vec4 fragColor;

void main() {
    vec2 noise = noise3D_to_2D(vec3(texCoord, time), space_noise_scale, time_noise_scale);//*2.0-1.0;
    noise = noise*deformation;
    vec2 newTexCoord = texCoord + noise; //deformed texCoord

    if (newTexCoord.x >1.0 || newTexCoord.y <0.0 ||newTexCoord.y >1.0 || newTexCoord.y <0.0){
        //if texCoord is outside the texture, set background
        fragColor = vec4(0.0, 0.0, 0.0, 1.0); 
    }
    else{
        float contour = isContourFromDepth(normalTexture, newTexCoord, window_w, window_h);
        fragColor = vec4(vec3(contour), 1.0);
    }

    
    

    
}