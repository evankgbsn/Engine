#version 460

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightUniformBufferObject {
	vec3 direction;
	vec3 color;
} light;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUVCoord;
layout(location = 0) out vec4 outColor;

void main(void)
{
	outColor = texture(texSampler, fragUVCoord);
}
