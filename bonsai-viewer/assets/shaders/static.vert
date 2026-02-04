#version 450

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec3 VertexTangent;
layout(location = 3) in vec3 VertexColor;
layout(location = 4) in vec2 VertexTexCoord;

layout(location = 0) out VERTEX_OUT {
    vec3 Color;
} VertexOut;

layout(binding = 0, std140) uniform CAMERA_UBO {
    mat4 ViewProjectMatrix;
} CameraUBO;

layout(binding = 1, std140) uniform TRANSFORM_UBO {
    mat4 ModelMatrix;
} TransformUBO;

void main() {
    vec4 position = TransformUBO.ModelMatrix * vec4(VertexPosition, 1.0);
    
    gl_Position = CameraUBO.ViewProjectMatrix * position;
    VertexOut.Color = VertexColor;
}
