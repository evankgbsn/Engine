#ifndef PIPELINE_LAYOUT_H
#define PIPELINE_LAYOUT_H

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;

class PipelineLayout
{

public:

	PipelineLayout(VulkanPhysicalDevice* device);

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