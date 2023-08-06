#version 460

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightsUniformBufferObject {
    vec4 ambient;
    vec4 direction;
    vec4 color;
} lightsUBO;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUVCoord;
layout(location = 0) out vec4 outColor;

vec4 DirectionalLight()
{
    float diffuseFactor = max(dot(fragNormal, -lightsUBO.direction.xyz), 0.0f);
    vec4 sampledFragColor = texture(texSampler, fragUVCoord);

    vec4 ambient = vec4(lightsUBO.ambient.xyz * sampledFragColor.xyz, sampledFragColor.a);
    vec4 diffuse = vec4(lightsUBO.color.xyz * diffuseFactor, sampledFragColor.a);

    diffuse *= sampledFragColor;
    ambient *= sampledFragColor;

    return ambient + diffuse;
};

void main(void)
{
	outColor = DirectionalLight();
}
