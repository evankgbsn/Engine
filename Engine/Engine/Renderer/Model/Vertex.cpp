#include "Vertex.h"

Vertex::Vertex() :
	weights(vec4(0.0f)),
	influences(ivec4(0)),
	position(vec3(0.0f)),
	normal(vec3(0.0f)),
	uv(vec2(0.0f))
{

}

Vertex::Vertex(const vec3& p, const vec3& n, const vec2& uvCoord) :
	weights(vec4(0.0f)),
	influences(ivec4(0)),
	position(p),
	normal(n),
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
			offsetof(Vertex, normal)
		},

		{
			2,
			0,
			VK_FORMAT_R32G32_SFLOAT,
			offsetof(Vertex, uv)
		},

		{
			3,
			0,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			offsetof(Vertex, weights)
		},

		{
			4,
			0,
			VK_FORMAT_R32G32B32A32_SINT,
			offsetof(Vertex, influences)
		}

	};

	return attributeDescriptions;
}
