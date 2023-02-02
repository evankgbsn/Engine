#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUVCoord;

void main(void) 
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition.z, inPosition.x, inPosition.y , 1.0);
    fragColor = inColor;
    fragUVCoord = inUV;
}