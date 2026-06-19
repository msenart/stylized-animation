#version 460 core

in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

uniform sampler2D tensorTexture; // sortie de la Pass 1
uniform vec2 screenSize;
uniform float sigma = 2.0f; // ex: 2.0

const int RADIUS = 5;

void main() {
    vec2 texel = 1.0 / screenSize;

    vec3 sum = vec3(0.0);
    float weightSum = 0.0;

    for (int dx = -RADIUS; dx <= RADIUS; dx++) {
        for (int dy = -RADIUS; dy <= RADIUS; dy++) {
            float d2 = float(dx*dx + dy*dy);
            float w = exp(-d2 / (2.0 * sigma * sigma));

            vec2 sampleUV = texCoord + vec2(float(dx), float(dy)) * texel;
            sum += texture(tensorTexture, sampleUV).rgb * w;
            weightSum += w;
        }
    }

    fragColor = vec4(sum / weightSum, 1.0);
//    fragColor = vec4(0.0,1.0,0.0,1.0);
}