#version 460 core
layout(location = 0) out vec4 FragColor0;
layout(location = 1) out uvec4 FragColor1;

uniform uint meshId;

void main() {
    FragColor0 = vec4(0.0, 1.0, 0.0, 1.0); // Bright green
    FragColor1 = uvec4(meshId, 0, 0, 0);
}
