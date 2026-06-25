#include "scene/Cinematic.h"

void Cinematic::uploadUniforms(const Shader& shader, float time){
    if(time<0.5){
        shader.set("myUniform", true);
    }
    else{
        shader.set("myUniform", false);
    }
    
}