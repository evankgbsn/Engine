#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

layout(location = 1) out vec2 fragUVCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragView;
layout(location = 4) out vec3 fragPos;

void main(void) 
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragNormal = inNormal;
    fragUVCoord = inUV;
    fragPos = inPosition;
    fragView = ubo.view[3].xyz;
}