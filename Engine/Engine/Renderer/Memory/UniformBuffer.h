#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include "Buffer.h"

#include <vulkan/vulkan.h>

class DescriptorSet;

class UniformBuffer : public Buffer
{

public:

	UniformBuffer() = delete;

	UniformBuffer(unsigned int sizeInBytes, unsigned int binding);

	~UniformBuffer();

	UniformBuffer(const UniformBuffer&) = delete;

	UniformBuffer& operator=(const UniformBuffer&) = delete;

	UniformBuffer(UniformBuffer&&) = delete;

	UniformBuffer& operator=(UniformBuffer&&) = delete;

	const VkDescriptorSetLayoutBinding& GetLayoutBinding() const;

	unsigned int Binding() const;

private:

	VkDescriptorSetLayoutBinding layoutBinding;

};

#endif // UNIFORMBUFFER_H