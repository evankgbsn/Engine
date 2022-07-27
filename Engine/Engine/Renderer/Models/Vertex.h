#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

class Vertex
{

public:

	Vertex();

	Vertex(const glm::vec2& position, const glm::vec4& color);

	Vertex(glm::vec2&& position, glm::vec4&& color);

	Vertex(const glm::vec2& position, glm::vec4&& color);

	Vertex(glm::vec2&& position, const glm::vec4& color);

	~Vertex();

	Vertex(const Vertex&);

	Vertex& operator=(const Vertex&);

	Vertex(Vertex&&) noexcept;

	Vertex& operator=(Vertex&&) noexcept;

	static VkVertexInputBindingDescription& GetBindingDescription();

	static std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions();

	glm::vec2 position = {0.0f, 0.0f};

	glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };

private:

};

#endif // VERTEX_H