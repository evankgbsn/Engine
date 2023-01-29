#include "Vertex.h"

Vertex::Vertex() :
	position(glm::vec2(0.0f, 0.0f)),
	color(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f))
{
}

Vertex::Vertex(const glm::vec2& p, const glm::vec4& c) :
	position(p),
	color(c)
{
}

Vertex::Vertex(glm::vec2&& p, glm::vec4&& c) :
	position(p),
	color(c)
{
}

Vertex::Vertex(const glm::vec2& p, glm::vec4&& c) :
	position(p),
	color(c)
{
}

Vertex::Vertex(glm::vec2&& p, const glm::vec4& c) :
	position(p),
	color(c)
{
}

Vertex::~Vertex()
{
}

Vertex::Vertex(const Vertex& vert) :
	position(vert.position),
	color(vert.color)
{
}

Vertex& Vertex::operator=(const Vertex& vert)
{
	position = vert.position;
	color = vert.color;
	return *this;
}

Vertex::Vertex(Vertex&& vert) noexcept :
	position(std::move(vert.position)),
	color(std::move(vert.color))
{
}

Vertex& Vertex::operator=(Vertex&& vert) noexcept
{
	position = std::move(vert.position);
	color = std::move(vert.color);
	return *this;
}

VkVertexInputBindingDescription& Vertex::GetBindingDescription()
{
	static VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.stride = sizeof(Vertex);

	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription>& Vertex::GetAttributeDescriptions()
{
	static std::vector<VkVertexInputAttributeDescription> attributeDescriptions =
	{
		{
			uint32_t(0), /* location */
			uint32_t(0), /* binding */
			VK_FORMAT_R32G32_SFLOAT, /* format */
			offsetof(Vertex, position) /* offset */
		},

		{
			uint32_t(1), /* location */
			uint32_t(0), /* binding */
			VK_FORMAT_R32G32B32A32_SFLOAT, /* format */
			offsetof(Vertex, color) /* offset */
		}
	};

	return attributeDescriptions;
}
