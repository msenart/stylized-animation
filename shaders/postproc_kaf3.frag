#version 460 core
#include "constants.glsl"

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform sampler2D tensorTexture;
uniform vec2 screenSize;
uniform int radius = 12;

const int N_SECTORS = 12;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 tensor = texture(tensorTexture, texCoord);

    float Exx = tensor.r;
    float Eyy = tensor.g;
    float Exy = tensor.b;

    float diff = Exx - Eyy;
    float theta = 0.5 * atan(2.0 * Exy, diff) + PI * 0.5;

    float lambda1 = 0.5*(Exx+Eyy) + 0.5*sqrt(diff*diff + 4.0*Exy*Exy);
    float lambda2 = 0.5*(Exx+Eyy) - 0.5*sqrt(diff*diff + 4.0*Exy*Exy);
    float anisotropy = (lambda1 - lambda2) / max(lambda1 + lambda2, 1e-4);

    mat2 rot   = mat2(cos(theta), -sin(theta), sin(theta), cos(theta));
    mat2 scale = mat2(1.0 + anisotropy, 0.0, 0.0, 1.0 - anisotropy);
    mat2 kernel = rot * scale;

    vec3  sectorMean[N_SECTORS];
    float sectorVar [N_SECTORS];
    float sectorW   [N_SECTORS];
    for (int i = 0; i < N_SECTORS; i++) {
        sectorMean[i] = vec3(0.0);
        sectorVar [i] = 0.0;
        sectorW   [i] = 0.0;
    }

    // Garde-fous anti division par zéro
    int   safeRadius = max(radius, 2);
    float radius2    = float(safeRadius * safeRadius);
    vec2  safeScreen = max(screenSize, vec2(1.0));
    vec2  texel      = 1.0 / safeScreen;

    for (int dx = -safeRadius; dx <= safeRadius; dx++) {
        for (int dy = -safeRadius; dy <= safeRadius; dy++) {

            float d2 = float(dx*dx + dy*dy);
            if (d2 > radius2) continue;

            vec2 offset = kernel * vec2(float(dx), float(dy));
            vec2 sampleUV = texCoord + offset * texel;
            vec3 col = texture(screenTexture, sampleUV).rgb;

            float angle = atan(float(dy), float(dx));
            int sector = int(floor((angle + PI) / (2.0*PI) * float(N_SECTORS))) % N_SECTORS;
            sector = max(sector, 0);

            float w = exp(-d2 / max(2.0 * radius2, 1e-4));

            sectorMean[sector] += col * w;
            sectorVar [sector] += dot(col, col) * w;
            sectorW   [sector] += w;
        }
    }

    vec3  result = vec3(0.0);
    float minVar = 1e9;

    for (int i = 0; i < N_SECTORS; i++) {
        if (sectorW[i] < 1e-6) continue;
        vec3  mean = sectorMean[i] / sectorW[i];
        float var  = sectorVar[i]  / sectorW[i] - dot(mean, mean);
        if (var < minVar) {
            minVar = var;
            result = mean;
        }
    }

    // Fallback si aucun secteur n'a accumulé de poids
    if (minVar >= 1e9) {
        result = texture(screenTexture, texCoord).rgb;
    }

    fragColor = vec4(result, 1.0);
}