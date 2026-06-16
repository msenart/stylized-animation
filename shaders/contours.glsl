//this file contains function to detect contours
//uniform float depthThreshold = 0.00394; //with depth of z non linear
uniform float depthThreshold = 0.5;
uniform float depthMult = 100.0;
uniform float normalThreshold = 0.5;




ivec2 getTexelFromTexCoord(vec2 texCoord, int window_w, int window_h){
    texCoord.x = texCoord.x*window_w;
    texCoord.y = texCoord.y*window_h;
    return ivec2(texCoord);
}

float isContourFromMeshId(usampler2D metadataTexture, vec2 texCoord, int window_w, int window_h){
    
    ivec2 texel = getTexelFromTexCoord(texCoord, window_w, window_h);
    uint texelMeshId = texelFetch(metadataTexture, texel, 0).x;

    //to have 1 texel thickness
    if(texelMeshId == 0){
        //float sum = 0.0;
        int x = texel.x;
        int y = texel.y;
        for (int i=-1; i<=1; i++){
            for (int j=-1; j<=1; j++){
                if(i!=0 && j!=0){
                    ivec2 neighborTexel = ivec2(x+i, y+j);
                    uint neighborMeshId = texelFetch(metadataTexture, neighborTexel, 0).x;
                    if(neighborMeshId!=0){
                        //sum = sum+1.0;
                        return 1.0;
                    }
                }
            }
        }
        
        // for (int x = texel.x-1; x<=texel.x+1; x++){
        //     for (int y = texel.y-1; x<=texel.y+1; y++){
        //         if(x!=0 || y!=0){
        //             ivec2 neighborTexel = ivec2(x, y);
        //             uint neighborMeshId = texelFetch(metadataTexture, neighborTexel, 0).x;
        //             if(neighborMeshId!=0){
        //                 sum = sum+1.0;
        //             }
        //         }
        //     }
        // }
        //return clamp(0.1*sum, 0.0, 1.0);
    }
    
    return 0.0;
    
}


float isContourFromDepth(sampler2D normalTexture, vec2 texCoord, int window_w, int window_h){
    ivec2 texel = getTexelFromTexCoord(texCoord, window_w, window_h);
    float depth = texelFetch(normalTexture, texel, 0).a;
    // if(depth==0.0){
    //     return 0.0;
    // }
    int x = texel.x;
    int y = texel.y;
    //float sum = 0.0;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            if(i!=0 && j!=0){
                ivec2 neighborTexel = ivec2(x+i, y+j);
                float neighborDepth = texelFetch(normalTexture, neighborTexel, 0).a;
                if(neighborDepth - depth> depthThreshold){
                    return 1.0;
                }
                //sum = sum + max(0, neighborDepth - depth - depthThreshold);
            }
        }
    }
    
    return 0.0;
    //return clamp(depthMult*sum, 0.0, 1.0);
}


float isContourFromNormal(sampler2D normalTexture, vec2 texCoord, int window_w, int window_h){
    ivec2 texel = getTexelFromTexCoord(texCoord, window_w, window_h);
    vec3 normal = texelFetch(normalTexture, texel, 0).rgb;//*2 - 1.0;
    int sum = 0;
    if(normal!=vec3(0.0, 0.0, 0.0)){
        int x = texel.x;
        int y = texel.y;
        for(int i=-1; i<=1; i++){
            for(int j=-1; j<=1; j++){
                if(i!=0 && j!=0){
                    ivec2 neighborTexel = ivec2(x+i, y+j);
                    vec3 neighborNormal = texelFetch(normalTexture, neighborTexel, 0).rgb;//*2 - 1.0;
                    if(dot(normal, neighborNormal)<normalThreshold && normal.z<neighborNormal.z ){//permet d'avoir un trai moins gros
                        sum = sum+1;
                        //return 0.5;
                    }
                    //sum = sum+1;
                }
            }
        }
        //sum = 1;
    }
    if(sum>=1){
        return 1.0;
    }
    
    return 0.0;
}


float testDepth(sampler2D normalTexture, vec2 texCoord, int window_w, int window_h){
    ivec2 texel = getTexelFromTexCoord(texCoord, window_w, window_h);
    float depth = texelFetch(normalTexture, texel, 0).a;
    // if(depth==0.0){
    //     return 0.0;
    // }
    int x = texel.x;
    int y = texel.y;
    float sum = 0.0;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            if(i!=0 && j!=0){
                ivec2 neighborTexel = ivec2(x+i, y+j);
                float neighborDepth = texelFetch(normalTexture, neighborTexel, 0).a;
                if(neighborDepth != depth){
                    return 1.0;
                }
            }
        }
    }
    return 0.0;
}