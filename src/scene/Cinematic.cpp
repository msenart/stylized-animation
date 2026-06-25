#include "scene/Cinematic.h"

void Cinematic::uploadUniforms(const Shader& shader, float time){
    //float
    std::vector<float> keyFrames = {1.3, 1.8, 1.9, 2.0, 2.1};
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