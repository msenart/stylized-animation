uvec4 getFragColor1(uint meshId){
    //return the value for the 2nd texture
    //r : MeshId
    //g : contour detection for alice
    //b, a : libre
    return uvec4(meshId, 0, 0, 0);
}

vec4 getFragColor2(vec3 normal, float depth){
    return vec4(normal, depth);
}