#version 460 core
#include "toon_shading_utilities.glsl"
out vec4 fragColor;

in vec3 fragPos;

const float theta = 0;
uniform vec3 sunDir = vec3(sin(theta),cos(theta),0);

const vec4 daySkyColor = vec4(174,237,255,1)/255;
const vec4 dayDiffuseSunColor = 0.1*vec4(195,241,255,1)/255;
const vec4 daySpecularSunColor = 0.3*vec4(219,245,255,1)/255;

const vec4 nightSkyColor = vec4(25,7,65,1)/255;
const vec4 nightDiffuseSunColor = 0.1*vec4(238,93,108,1)/255;
const vec4 nightSpecularSunColor = 0.3*vec4(238,175,97,1)/255;

vec4 skyColor = mix(daySkyColor,nightSkyColor,1-(cos(theta)+1)/2);
vec4 diffuseSunColor = mix(dayDiffuseSunColor,nightDiffuseSunColor,1-(cos(theta)+1)/2);
vec4 specularSunColor = mix(daySpecularSunColor,nightSpecularSunColor,1-(cos(theta)+1)/2);

void main(){
    vec3 fragDirection = normalize(fragPos);
    float light = max(dot(sunDir,fragDirection), 0);
    fragColor = vec4(vec3(1,1,1)*light,0);
    fragColor = skyColor + diffuseSunColor*sin_smoothstep(light, 0.0,0.5)
    +specularSunColor*sin_smoothstep(light,0.7,0.9);
}