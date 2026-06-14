//contains function to detect contours
uniform float stepX = 0.005;
uniform float stepY = 0.005;

bool isContourFromMeshId(usampler2D metadataTexture, vec2 texCoord, int window_w, int window_h){
    texCoord.x = texCoord.x*window_w;
    texCoord.y = texCoord.y*window_h;
    ivec2 texel = ivec2(texCoord);
    uvec4 metadata = texelFetch(metadataTexture, texel, 0);
    uint meshID = metadata.x;
    return meshID == 0;
    
}