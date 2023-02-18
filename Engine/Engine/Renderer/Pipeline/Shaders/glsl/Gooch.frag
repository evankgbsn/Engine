#version 460

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 1) in vec2 fragUVCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragView;
layout(location = 4) in vec3 fragPos;
layout(location = 0) out vec4 outColor;

void main(void)
{
	// Light direction
	vec3 l = vec3(0.0f, -1.0f, 0.0f);

	// Warm and cool to interpolate between
	vec3 cool = vec3(0.0f, 0.0f, 0.55f) + 0.25f * texture(texSampler, fragUVCoord).xyz;
	vec3 warm = vec3(0.3f, 0.3f, 0.0f) + 0.25f * texture(texSampler, fragUVCoord).xyz;

	float t = (dot(fragNormal, l) + 1) / 2.0f;
	vec3 r = 2 * (dot(fragNormal, l)) * fragNormal - 1.0f;
	float s = clamp((100.0f * (dot(r, (fragPos - fragView))) - 97.0f) , 0.0f, 1.0f);

	outColor = vec4(s * vec3(1,1,1) + (1.0f - s) * (t * warm + (1.0f - t) * cool), 1.0f);
}
