#include "PipelineLayout.h"

#include <stdexcept>

#include "../../Utils/Logger.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

PipelineLayout::PipelineLayout(VulkanPhysicalDevice* d) :
	device(d)
{
	VkPipelineLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = 0;
	createInfo.pSetLayouts = nullptr;
	createInfo.pushConstantRangeCount = 0;
	createInfo.pPushConstantRanges = nullptr;

	VkResult result = vkCreatePipelineLayout(device->GetLogicalDevice(), &createInfo, nullptr, &layout);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create pipeline layout in PipelineLayout::PipelineLayout"), Logger::Category::Error);
		throw std::runtime_error("Failed to create pipeline layout in PipelineLayout::PipelineLayout");
		return;
	}
}

PipelineLayout::~PipelineLayout()
{
	vkDestroyPipelineLayout(device->GetLogicalDevice(), layout, nullptr);
}

const VkPipelineLayout& PipelineLayout::operator*() const
{
	return layout;
}
