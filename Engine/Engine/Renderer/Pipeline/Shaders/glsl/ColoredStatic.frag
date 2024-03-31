#version 460

layout(binding = 1) uniform ColorUniformBufferObject {
    vec4 color;
} cubo;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUVCoord;
layout(location = 0) out vec4 outColor;

void main(void)
{
	outColor = cubo.color;
}



