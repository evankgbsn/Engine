#include "Vertex.h"

Vertex::Vertex() :
	position(vec3(0)),
	color(vec3(0)),
	uv(vec2(0))
{

}

Vertex::Vertex(const vec3& p, const vec3& c, const vec2& uvCoord) :
	position(p),
	color(c),
	uv(uvCoord)
{

}

Vertex::~Vertex()
{

}

VkVertexInputBindingDescription& Vertex::GetVertexInputBindingDescription()
{
	static VkVertexInputBindingDescription bindingDescription = 
	{
		0, // Binding
		sizeof(Vertex), // Stride
		VK_VERTEX_INPUT_RATE_VERTEX // Input Rate
	};

	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription>& Vertex::GetVertexInputAttributeDescriptions()
{
	static std::vector<VkVertexInputAttributeDescription> attributeDescriptions =
	{
		{
			0, // Location;
			0, // Binding;
			VK_FORMAT_R32G32B32_SFLOAT, // Format;
			offsetof(Vertex, position) // Offset;
		},
		
		{
			1,
			0, 
			VK_FORMAT_R32G32B32_SFLOAT,
			offsetof(Vertex, color)
		},

		{
			2,
			0,
			VK_FORMAT_R32G32_SFLOAT,
			offsetof(Vertex, uv)
		}

	};

	return attributeDescriptions;
}
