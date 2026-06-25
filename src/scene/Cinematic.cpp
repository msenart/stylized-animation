#include "scene/Cinematic.h"

void Cinematic::uploadUniforms(const Shader& shader, float time){
    //float
    float animation_duration = 3.16; //3.2
    while(time>animation_duration){
        time -= animation_duration;
    }
    std::vector<float> keyFrames = {1.35, 1.8, 1.9, 2.0, 2.1}; //1.3
    std::vector<int> effects = {0, 2, 1, 2, 1};
    int effect = 0;
    for(int i=0; i<keyFrames.size(); i++){
        if(time<keyFrames[i]){
            effect = effects[i];
            break;
        }
    }
    shader.set("effect", effect);

    
}