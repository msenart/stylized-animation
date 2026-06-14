//contains function to detect contours
uniform float stepX = 0.005;
uniform float stepY = 0.005;


bool isContourFromMeshId(usampler2D metadataTexture, vec2 texCoord, int window_w, int window_h){
    texCoord.x = texCoord.x*window_w;
    texCoord.y = texCoord.y*window_h;
    ivec2 texel = ivec2(texCoord);
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
                        return true;
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
    
    return false;
    
}