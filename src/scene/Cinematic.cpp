#include "scene/Cinematic.h"

void Cinematic::uploadUniforms(const Shader& shader, float time){
    if(time>1.0 && time<1.5){
        shader.set("effect", 1);
    }
    else if(time>1.7 && time<2.0){
        shader.set("effect", 2);
    }
    else{
        shader.set("effect", 0);
    }
    
}