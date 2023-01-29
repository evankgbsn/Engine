#include "StagingBuffer.h"

#include "../../Utils/Logger.h"

#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

#include <stdexcept>

StagingBuffer::StagingBuffer(unsigned int sizeInBytes) :
	Buffer(sizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
{
	
}

StagingBuffer::~StagingBuffer()
{

}