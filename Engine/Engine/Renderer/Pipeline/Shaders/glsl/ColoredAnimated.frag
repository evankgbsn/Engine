#version 460

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 3) uniform LightsUniformBufferObject {
    vec4 ambient;
    vec4 direction;
    vec4 color;
} lightsUBO;

layout(binding = 4) uniform ColorUBO {
    vec4 color;
} colorUBO;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUVCoord;
layout(location = 0) out vec4 outColor;

vec4 DirectionalLight()
{
    float diffuseFactor = max(dot(fragNormal, -lightsUBO.direction.xyz), 0.0f);

    vec4 ambient = vec4(lightsUBO.ambient.xyz * colorUBO.color.xyz, colorUBO.color.a);
    vec4 diffuse = vec4(lightsUBO.color.xyz * diffuseFactor, colorUBO.color.a);

    diffuse *= colorUBO.color;
    ambient *= colorUBO.color;

    return ambient + diffuse;
};

void main(void)
{
	outColor = DirectionalLight();
}