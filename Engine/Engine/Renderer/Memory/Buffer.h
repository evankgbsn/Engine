#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vk_mem_alloc.h>

class Buffer
{

public:

	virtual const VkBuffer& operator()() const;

	void Map(void* data, unsigned int sizeInBytes);

	void PersistentMap();

	void Unmap();

	void SetData(void* data);

	void CopyFrom(const Buffer& buffer);

	unsigned int Size() const;

protected:

	Buffer() = delete;

	Buffer(unsigned int sizeInBytes, VkBufferUsageFlags usageFlags, VmaAllocationCreateFlagBits memoryProperties);

	~Buffer();

	Buffer(const Buffer&) = delete;

	Buffer& operator=(const Buffer&) = delete;

	Buffer(Buffer&&) = delete;

	Buffer& operator=(Buffer&&) = delete;

	unsigned int FindMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);

	VkBufferCreateInfo bufferCreateInfo;

	VkBuffer buffer;

	VmaAllocationCreateInfo memoryAllocationCreateInfo;

	VmaAllocation memory;

	VkBufferUsageFlags usageFlags;

	VkMemoryPropertyFlags memoryPropertyFlags;

	void* data;

private:


};


#endif // BUFFER_H