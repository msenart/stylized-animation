#version 460 core

in vec2 texCoord;

uniform sampler2D screenTexture;
uniform vec2 screenSize;

layout(location = 0) out vec4 fragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / screenSize;
    vec2 texel = 1.0 / screenSize;

    // Gradients via Sobel
    vec3 s00 = texture(screenTexture, uv + texel * vec2(-1,-1)).rgb;
    vec3 s10 = texture(screenTexture, uv + texel * vec2( 0,-1)).rgb;
    vec3 s20 = texture(screenTexture, uv + texel * vec2( 1,-1)).rgb;
    vec3 s01 = texture(screenTexture, uv + texel * vec2(-1, 0)).rgb;
    vec3 s21 = texture(screenTexture, uv + texel * vec2( 1, 0)).rgb;
    vec3 s02 = texture(screenTexture, uv + texel * vec2(-1, 1)).rgb;
    vec3 s12 = texture(screenTexture, uv + texel * vec2( 0, 1)).rgb;
    vec3 s22 = texture(screenTexture, uv + texel * vec2( 1, 1)).rgb;

    vec3 gx = -s00 + s20 - 2.0*s01 + 2.0*s21 - s02 + s22;
    vec3 gy = -s00 - 2.0*s10 - s20 + s02 + 2.0*s12 + s22;

    // Luminance des gradients
    float Ex = dot(gx, vec3(0.299, 0.587, 0.114));
    float Ey = dot(gy, vec3(0.299, 0.587, 0.114));

    // Structure tensor : [Ex², ExEy ; ExEy, Ey²]
    fragColor = vec4(Ex*Ex, Ey*Ey, Ex*Ey, 1.0);
}