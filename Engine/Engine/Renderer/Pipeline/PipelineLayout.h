#ifndef PIPELINE_LAYOUT_H
#define PIPELINE_LAYOUT_H

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
class DescriptorSetLayout;

class PipelineLayout
{

public:

	PipelineLayout(VulkanPhysicalDevice* device, const DescriptorSetLayout* const descriptorSetLayout);

	~PipelineLayout();

	const VkPipelineLayout& operator*() const;

private:

	PipelineLayout() = delete;

	PipelineLayout(const PipelineLayout&) = delete;

	PipelineLayout& operator=(const PipelineLayout&) = delete;
	
	PipelineLayout(const PipelineLayout&&) = delete;

	PipelineLayout& operator=(const PipelineLayout&&) = delete;

	VkPipelineLayout layout = VK_NULL_HANDLE;

	VulkanPhysicalDevice* const device = nullptr;
};


#endif // PIPELINE_LAYOUT_H