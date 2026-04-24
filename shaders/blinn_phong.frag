#version 460 core

in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;

out vec4 fragColor;

void main() {
    vec3 norm     = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir  = normalize(viewPos  - fragPos);
    vec3 halfDir  = normalize(lightDir + viewDir);

    float ambient = 0.15;
    float diff    = max(dot(norm, lightDir), 0.0);
    float spec    = pow(max(dot(norm, halfDir), 0.0), 32.0);

    vec3 result = (ambient + diff + 0.5 * spec) * lightColor * objectColor;
    fragColor   = vec4(result, 1.0);
}