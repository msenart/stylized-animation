uvec4 getFragColor1(uint meshId){
    //return the value for the 2nd texture
    //r : MeshId
    //g : contour detection for alice
    //b, a : libre
    return uvec4(meshId, 0, 0, 0);
}

vec4 getFragColor2(vec3 normal, float z){
    // float depth = 0.0;
    // if(z!=0.0){
    //     depth = 1/z;
    // }
    //becuase -1.0<normal<1.0 and it will clamp to 0.0, 1.0
    normal = (normal+1.0)/2;
    return vec4(normal, z);
}