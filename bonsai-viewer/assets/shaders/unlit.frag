#version 450

layout(location = 1) in VERTEX_OUT {
    vec3 Color;
} VertexOut;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(VertexOut.Color, 1.0);
}
