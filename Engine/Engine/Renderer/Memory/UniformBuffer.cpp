#include "UniformBuffer.h"

#include "../Pipeline/Shaders/DescriptorSetManager.h"

UniformBuffer::UniformBuffer(unsigned int sizeInBytes, unsigned int binding) :
	Buffer(sizeInBytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT),
	layoutBinding({})
{
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
}

UniformBuffer::~UniformBuffer()
{

}

const VkDescriptorSetLayoutBinding& UniformBuffer::GetLayoutBinding() const
{
	return layoutBinding;
}

unsigned int UniformBuffer::Binding() const
{
	return layoutBinding.binding;
}
