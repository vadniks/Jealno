
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

const float near_plane = 1.0;
const float far_plane = 7.5;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
    FragColor = vec4(vec3(LinearizeDepth(texture(depthMap, TexCoords).r) / far_plane), 1.0);
}
