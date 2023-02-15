#include "DescriptorSetLayout.h"

#include "../../Renderer.h"
#include "../../Vulkan/VulkanPhysicalDevice.h"
#include "../../Vulkan/Utils/VulkanUtils.h"
#include "../../Memory/UniformBuffer.h"
#include "../../Memory/Image.h"

DescriptorSetLayout::DescriptorSetLayout() :
	layout(VK_NULL_HANDLE),
	layoutBindings(std::vector<VkDescriptorSetLayoutBinding>()),
	createInfo({})
{
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (layout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), layout, nullptr);
	}
}

const VkDescriptorSetLayout& DescriptorSetLayout::operator()() const
{
	return layout;
}

void DescriptorSetLayout::AddLayoutBinding(const VkDescriptorSetLayoutBinding& layoutBinding)
{
	layoutBindings.push_back(layoutBinding);
}

void DescriptorSetLayout::Finalize()
{
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	createInfo.pBindings = layoutBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &createInfo, nullptr, &layout);

	VulkanUtils::CheckResult(result, true, true, "Failed to create DescriptorSetLayout DescriptorSetLayout::Finalize().");
}

const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayout::GetLayoutBindings() const
{
	return layoutBindings;
}
