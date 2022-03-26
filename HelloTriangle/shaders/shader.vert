#version 450

// The inPosition and inColor variables are vertex attributes. 
// They're properties that are specified per-vertex in the vertex buffer, 
// just like we manually specified a position and color per vertex using the two arrays. 
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}