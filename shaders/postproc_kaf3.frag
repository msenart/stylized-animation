#version 460 core
#include "constants.glsl"

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform sampler2D tensorTexture;
uniform vec2 screenSize;
uniform int radius = 10;

const int N_SECTORS = 8;

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
    float anisotropy = (lambda1 - lambda2) / (lambda1 + lambda2 + 1e-6);

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

    vec2 texel = 1.0 / screenSize;
    float radius2 = float(radius * radius);

    for (int dx = -radius; dx <= radius; dx++) {
        for (int dy = -radius; dy <= radius; dy++) {

            float d2 = float(dx*dx + dy*dy);
            if (d2 > radius2) continue;

            vec2 offset = kernel * vec2(float(dx), float(dy));
            vec2 sampleUV = texCoord + offset * texel;
            vec3 col = texture(screenTexture, sampleUV).rgb;

            float angle = atan(float(dy), float(dx));
            int sector = int(floor((angle + PI) / (2.0*PI) * float(N_SECTORS))) % N_SECTORS;

            float w = exp(-d2 / (2.0 * radius2));

            // Inliné directement, plus de fonction inout sur array element
            if (sector == 0) { sectorMean[0] += col*w; sectorVar[0] += dot(col,col)*w; sectorW[0] += w; }
            else if (sector == 1) { sectorMean[1] += col*w; sectorVar[1] += dot(col,col)*w; sectorW[1] += w; }
            else if (sector == 2) { sectorMean[2] += col*w; sectorVar[2] += dot(col,col)*w; sectorW[2] += w; }
            else if (sector == 3) { sectorMean[3] += col*w; sectorVar[3] += dot(col,col)*w; sectorW[3] += w; }
            else if (sector == 4) { sectorMean[4] += col*w; sectorVar[4] += dot(col,col)*w; sectorW[4] += w; }
            else if (sector == 5) { sectorMean[5] += col*w; sectorVar[5] += dot(col,col)*w; sectorW[5] += w; }
            else if (sector == 6) { sectorMean[6] += col*w; sectorVar[6] += dot(col,col)*w; sectorW[6] += w; }
            else if (sector == 7) { sectorMean[7] += col*w; sectorVar[7] += dot(col,col)*w; sectorW[7] += w; }
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

//    fragColor = vec4(result, 1.0);
    fragColor = texture(tensorTexture,texCoord);
//    fragColor = vec4(1.0,.0,.0,1.0);
}