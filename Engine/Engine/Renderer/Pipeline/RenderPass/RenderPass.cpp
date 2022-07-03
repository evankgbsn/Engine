#include "RenderPass.h"

#include <stdexcept>

#include "../../../Utils/Logger.h"
#include "../../Renderer.h"
#include "../../Windows/Window.h"
#include "../../Vulkan/VulkanPhysicalDevice.h"

RenderPass::RenderPass(const Window& w) :
	window(w)
{
	colorAttachment.format = window.GetSurfaceFormat().format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;

	VkResult result = vkCreateRenderPass(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &createInfo, nullptr, &renderPass);
	
	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create a render pass."), Logger::Category::Error);
		throw std::runtime_error("Failed to create a render pass.");
	}
}

RenderPass::~RenderPass()
{
	vkDestroyRenderPass(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), renderPass, nullptr);
}
