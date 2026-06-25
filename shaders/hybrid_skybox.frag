#version 460 core
#include "toon_shading_utilities.glsl"
out vec4 fragColor;

in vec3 fragPos;

const float theta = 1.3;
uniform vec3 sunDir = vec3(0,cos(theta),-sin(theta));

const vec4 daySkyColor = vec4(0.95,0.37,0.17,1)*2;
const vec4 dayDiffuseSunColor = 0.2*vec4(1.0,0.65,0.4,1);
const vec4 daySpecularSunColor = 0.3*vec4(1.0,1.0,0.8,1);

const vec4 nightSkyColor = vec4(25,7,65,1)/255;
const vec4 nightDiffuseSunColor = 0.1*vec4(238,93,108,1)/255;
const vec4 nightSpecularSunColor = 0.3*vec4(238,175,97,1)/255;

//vec4 skyColor = mix(daySkyColor,nightSkyColor,1-(cos(theta)+1)/2);
//vec4 diffuseSunColor = mix(dayDiffuseSunColor,nightDiffuseSunColor,1-(cos(theta)+1)/2);
//vec4 specularSunColor = mix(daySpecularSunColor,nightSpecularSunColor,1-(cos(theta)+1)/2);

vec4 skyColor = daySkyColor;
vec4 diffuseSunColor = dayDiffuseSunColor;
vec4 specularSunColor = nightSpecularSunColor;

void main(){
    vec3 fragDirection = normalize(fragPos);
    float light = max(dot(sunDir,fragDirection), 0);
    fragColor = vec4(vec3(1,1,1)*light,0);
    fragColor = skyColor + diffuseSunColor*sin_smoothstep(light, 0.2,0.5)
    +specularSunColor*sin_smoothstep(light,0.88,0.90);
}