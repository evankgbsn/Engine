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

	void AddLayoutBinding(const VkDescriptorSetLayoutBinding& layoutBinding);

	void Finalize();

	const std::vector<VkDescriptorSetLayoutBinding>& GetLayoutBindings() const;

private:

	VkDescriptorSetLayout layout;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	VkDescriptorSetLayoutCreateInfo createInfo;

};

#endif // DESCRIPTORSETLAYOUT_h