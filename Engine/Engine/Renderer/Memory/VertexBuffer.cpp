#include "VertexBuffer.h"

#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"
#include "../../Utils/Logger.h"

#include <stdexcept>
#include <sstream>

VertexBuffer::VertexBuffer(unsigned int sizeInBytes) :
	Buffer(sizeInBytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, static_cast<VmaAllocationCreateFlagBits>(0))
{
	
}

VertexBuffer::~VertexBuffer()
{
	
}
