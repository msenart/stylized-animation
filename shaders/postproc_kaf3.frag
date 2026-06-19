#version 460 core
#include "constants.glsl"

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform vec2 screenSize;
uniform sampler2D tensorTexture; // résultat smoothé
uniform int radius = 6;              // ex: 6

const int N_SECTORS = 8; // secteurs angulaires

layout(location = 0) out vec4 fragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / screenSize;
    vec4 tensor = texture(tensorTexture, uv);

    float Exx = tensor.r;
    float Eyy = tensor.g;
    float Exy = tensor.b;

    // Eigenvectors du tensor → orientation principale
    float diff = Exx - Eyy;
    float theta = 0.5 * atan(2.0 * Exy, diff);

    // Anisotropie → élongation de l'ellipse
    float lambda1 = 0.5*(Exx+Eyy) + 0.5*sqrt(diff*diff + 4.0*Exy*Exy);
    float lambda2 = 0.5*(Exx+Eyy) - 0.5*sqrt(diff*diff + 4.0*Exy*Exy);
    float anisotropy = (lambda1 - lambda2) / (lambda1 + lambda2 + 1e-6);

    // Matrice de rotation selon l'orientation du tensor
    mat2 rot = mat2(cos(theta), -sin(theta), sin(theta), cos(theta));
    // Scale anisotrope : étire dans la direction de l'edge
    mat2 scale = mat2(1.0 + anisotropy, 0.0, 0.0, 1.0 - anisotropy);
    mat2 kernel = rot * scale;

    // Accumulateurs par secteur
    vec3  sectorMean[N_SECTORS];
    float sectorVar [N_SECTORS];
    float sectorW   [N_SECTORS];
    for (int i = 0; i < N_SECTORS; i++) {
        sectorMean[i] = vec3(0.0);
        sectorVar [i] = 0.0;
        sectorW   [i] = 0.0;
    }

    vec2 texel = 1.0 / screenSize;

    for (int dx = -radius; dx <= radius; dx++) {
        for (int dy = -radius; dy <= radius; dy++) {
            vec2 offset = kernel * vec2(float(dx), float(dy));
            if (length(offset) > float(radius)) continue; // kernel elliptique

            vec2 sampleUV = uv + offset * texel;
            vec3 col = texture(screenTexture, sampleUV).rgb;

            // Secteur angulaire de cet échantillon
            float angle = atan(offset.y, offset.x);
            int sector = int(floor((angle + PI) / (2.0*PI) * float(N_SECTORS))) % N_SECTORS;

            // Poids gaussien
            float w = exp(-dot(offset, offset) / (2.0 * float(radius*radius)));

            sectorMean[sector] += col * w;
            sectorVar [sector] += dot(col, col) * w;
            sectorW   [sector] += w;
        }
    }

    // Sélectionne le secteur avec la variance la plus faible
    vec3  result  = vec3(0.0);
    float minVar  = 1e9;

    for (int i = 0; i < N_SECTORS; i++) {
        if (sectorW[i] < 1e-6) continue;
        vec3  mean = sectorMean[i] / sectorW[i];
        float var  = sectorVar[i]  / sectorW[i] - dot(mean, mean);
        if (var < minVar) {
            minVar = var;
            result = mean;
        }
    }

    fragColor = vec4(result, 1.0);
}
