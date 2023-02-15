#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

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

	bool operator==(const Vertex&) const;

	bool operator!=(const Vertex&) const;

	static VkVertexInputBindingDescription& GetVertexInputBindingDescription();

	static std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescriptions();

	vec4& GetWeights();

	ivec4& GetInfluences();

	vec3& GetPosition();

	vec3& GetNormal();

	vec2& GetUV();

	const vec4& GetWeights() const;

	const ivec4& GetInfluences() const;

	const vec3& GetPosition() const;

	const vec3& GetNormal() const;

	const vec2& GetUV() const;

private:

	vec4 weights;
	ivec4 influences;
	vec3 position;
	vec3 normal;
	vec2 uv;
};

namespace std {
	template<>
	struct hash<Vertex> {
		size_t operator()(const Vertex& vertex) const
		{
			return	(hash<glm::vec3>()(vertex.GetPosition()) ^ 
					(hash<glm::vec3>()(vertex.GetNormal()) << 1) ^ 
					(hash<glm::vec2>()(vertex.GetUV()) << 1) ^
					(hash<glm::vec2>()(vertex.GetWeights()) << 1) ^
					(hash<glm::vec2>()(vertex.GetInfluences()) << 1));
		}
	};
};

#endif // VERTEX_H