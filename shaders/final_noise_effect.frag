#version 460 core


uniform float deformation = 0.0;
uniform vec3 background_color = vec3(0.1, 0.9, 0.1);

#include "perlin_noise.glsl"

uniform sampler2D sceneTexture; //the scene has been renderered in this texture
uniform usampler2D metadataTexture; //the metadata has been renderered in this texture
                                    //notice the u before "sampler2D" that indicates that
                                    //sampler type is uvec

uniform float time = 0.5; //value between 0 and 1


in vec2 texCoord;


out vec4 fragColor;

void main() {
    

    vec2 noise = noise3D_to_2D(vec3(texCoord, time), 10.0)*deformation;
    //noise = vec2(0.1, 0.1);
    vec2 newTexCoord = texCoord + noise; //deformed texCoord

    if (newTexCoord.x >1.0 || newTexCoord.y <0.0 ||newTexCoord.y >1.0 || newTexCoord.y <0.0){
        //if texCoord is outside the texture, set background
        fragColor = vec4(background_color, 1.0);
    }
    else{
        //draw the scene with deformed value
        uvec4 metadata = texture(metadataTexture, newTexCoord);
        uint meshId = metadata.x;
        if(meshId == 0){
            fragColor = vec4(background_color, 1.0);
        }
        else{
            fragColor = vec4(texture(sceneTexture, newTexCoord).rgb, 1.0);
        }
    }
    fragColor = vec4(noise3D_to_2D(vec3(texCoord, time), 4.0), 1.0, 1.0);

}