#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H

#include <vulkan/vulkan.h>

#include <vector>

class UniformBuffer;
class Image;

class DescriptorSetLayout
{

public:

	DescriptorSetLayout();

	~DescriptorSetLayout();

	DescriptorSetLayout(const DescriptorSetLayout&) = delete;

	DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
	
	DescriptorSetLayout(DescriptorSetLayout&&) = delete;

	DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

	const VkDescriptorSetLayout& operator()() const;

	void AddUniformBufferLayout(const UniformBuffer& uniformBuffer);

	void AddImageLayout(const Image& image);

	void Finalize();

private:

	VkDescriptorSetLayout layout;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	VkDescriptorSetLayoutCreateInfo createInfo;

};

#endif // DESCRIPTORSETLAYOUT_h