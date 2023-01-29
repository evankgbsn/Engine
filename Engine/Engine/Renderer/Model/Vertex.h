#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vector>

using namespace glm;

class Vertex
{
public:

	Vertex();

	Vertex(const vec3& position, const vec3& color, const vec2& uvCoord);

	~Vertex();

	Vertex(const Vertex&) = default;

	Vertex& operator=(const Vertex&) = default;

	Vertex(Vertex&&) = default;

	Vertex& operator=(Vertex&&) = default;

	static VkVertexInputBindingDescription& GetVertexInputBindingDescription();

	static std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescriptions();

private:

	vec4 weights;
	ivec4 influences;
	vec3 position;
	vec3 normal;
	vec2 uv;
};

#endif // VERTEX_H