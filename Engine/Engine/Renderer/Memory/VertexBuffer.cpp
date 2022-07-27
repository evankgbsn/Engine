#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(uint32_t size) :
	Buffer(size)
{
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.flags = 0;

	CreateBuffer();
}

VertexBuffer::~VertexBuffer()
{
}
