#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H

#include <vulkan/vulkan.h>

class UniformBuffer;
class DescriptorSetLayout;
class Image;

class DescriptorSet
{

public:

	DescriptorSet() = delete;

	DescriptorSet(const VkDescriptorPool& descriptorPool, const UniformBuffer& uniformBuffer, const Image& image);

	~DescriptorSet();

	DescriptorSet(const DescriptorSet&) = delete;

	DescriptorSet& operator=(const DescriptorSet&) = delete;

	DescriptorSet(DescriptorSet&&) = delete;

	DescriptorSet& operator=(DescriptorSet&&) = delete;

	const VkDescriptorSet& operator()() const;

	const DescriptorSetLayout* const GetLayout() const;

private:

	DescriptorSetLayout* layout;

	VkDescriptorSetAllocateInfo allocInfo;

	VkDescriptorSet descriptorSet;

	const VkDescriptorPool& pool;
};


#endif // DESCRIPTORSET_H