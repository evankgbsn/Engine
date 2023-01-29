#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(unsigned int sizeInBytes) :
	Buffer(sizeInBytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, static_cast<VmaAllocationCreateFlagBits>(0))
{

}

IndexBuffer::~IndexBuffer()
{

}