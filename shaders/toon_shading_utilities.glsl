#version 120

#include "constants.glsl"

/*
Function that makes cross-hatching technique.
*/
float crosshatching(in vec2 uv, in float grid_scale, in float line_thickness){
    float d = length(fract(uv.x * grid_scale) - 0.5);
    return step(d, line_thickness);
}


/*
Function that makes little dots as shaders.
*/
float halftone(in vec2 uv, in float grid_scale, in float light_intensity){
    float d = length(fract(uv * grid_scale) - 0.5);
    return step(d, (light_intensity*light_intensity)*INV_SQRT_2);
}

float exp_smoothstep(in float x, in float speed){
    return x < 0 ? exp(speed*x)/(1+exp(speed*x)) : 1/(1+exp(-speed*x));
}

float sin_smoothstep(in float x, in float b){
    if (x < -b){
        return 0;
    }
    else if (x > b) {
        return 1;
    }
    else{
        return (sin((x/(2*b))*PI) + 1)/2.f;
    }
}

float sin_smoothstep(in float x, in float lb, in float ub){
    if (x < lb){
        return 0;
    }
    else if (x > ub) {
        return 1;
    }
    else{
        return (sin(((x-lb)/(ub-lb)-0.5f)*PI)+1)/2;
    }
}

float diffuse_toon(in float x, in float lb1, in float ub1, in float lb2, in float ub2, in float plateau){
    return plateau * sin_smoothstep(x, lb1, ub1) + (1 - plateau)* sin_smoothstep(x, lb2, ub2);
}