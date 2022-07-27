#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>

class Buffer
{
public:

	VkBuffer& operator*();

	~Buffer();

	void SetData(void* data);

protected:

	Buffer(uint32_t size);

	Buffer(const Buffer&) = delete;

	Buffer& operator=(const Buffer&) = delete;

	Buffer(Buffer&&) = delete;

	Buffer& operator=(Buffer&&) = delete;

	void FindMemoryType();

	void AllocateMemory();

	void CreateBuffer();

	VkBufferCreateInfo createInfo = {};

private:

	VkBuffer buffer = VK_NULL_HANDLE;

	VkDeviceMemory memory = VK_NULL_HANDLE;

	VkMemoryRequirements memoryReqs = {};

	uint32_t memoryType = 0;
};






#endif // BUFFER_H