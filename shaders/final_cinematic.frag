#version 460 core

#include "contours.glsl"
#include "perlin_noise.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform sampler2D normalTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec

uniform int effect=0; //tell which NPR effect to use (0 : use default texture)
uniform float time;

//window (for contour detection)
uniform int window_w;
uniform int window_h;

in vec2 texCoord;


out vec4 fragColor;


vec4 noisy_contours_getFragColor(){
    float deformation = 0.02; 
    float space_noise_scale = 50.0; 
    float time_noise_scale = 20.0; 
    vec2 noise = noise3D_to_2D(vec3(texCoord, time), space_noise_scale, time_noise_scale);//*2.0-1.0;
    noise = noise*deformation;
    vec2 newTexCoord = texCoord + noise; //deformed texCoord


    if (newTexCoord.x >1.0 || newTexCoord.y <0.0 ||newTexCoord.y >1.0 || newTexCoord.y <0.0){
        //if texCoord is outside the texture, set background
        return vec4(0.0, 0.0, 0.0, 1.0); 
    }
    else{
        float contour = isContourFromDepth(normalTexture, newTexCoord, window_w, window_h);
        return vec4(vec3(contour), 1.0);
    }
}

vec4 noise_effect_getFragColor(){
    float deformation = 0.03; //0.05 //0.02
    float space_noise_scale = 12.0; //10 //15
    float time_noise_scale = 3.0; //10 //15
    vec2 noise = noise3D_to_2D(vec3(texCoord, time), space_noise_scale, time_noise_scale);
    vec3 background = vec3(noise.x, noise.y, 1.0);
    noise = noise*deformation;
    //noise = vec2(0.1, 0.1);
    vec2 newTexCoord = texCoord + noise; //deformed texCoord
    // newTexCoord.x = clamp(newTexCoord.x, 0.0, 1.0);
    // newTexCoord.y = clamp(newTexCoord.y, 0.0, 1.0);

    if (newTexCoord.x >1.0 || newTexCoord.y <0.0 ||newTexCoord.y >1.0 || newTexCoord.y <0.0){
        //if texCoord is outside the texture, set background
        return vec4(background, 1.0); //find a better solution ? (scale the image to have what is outside)
                                                //pixelisé mais tant pis? ou alors meileur resolution du framebuffer d'avant mais
                                                //ça devient compliqué
    }
    else{
        //draw the scene with deformed value
        uvec4 metadata = texture(metadataTexture, newTexCoord);
        uint meshId = metadata.x;
        if(meshId == 0){
            return vec4(background, 1.0);
        }
        else{
            return vec4(texture(sceneTexture, newTexCoord).rgb, 1.0);
        }
    }
}


void main() {
    if(effect==1){
        fragColor = noisy_contours_getFragColor();
    }
    else if (effect==2){
        fragColor = noise_effect_getFragColor();
    }
    else{
        fragColor = vec4(texture(sceneTexture, texCoord).rgb, 1.0);
    }
}