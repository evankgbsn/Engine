#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <vulkan/vulkan.h>

#include "../../Windows/Window.h"

class RenderPass
{

public:

	RenderPass(const Window& window);

	~RenderPass();

	const VkRenderPass& operator*() { return renderPass; };

private:

	RenderPass() = delete;

	RenderPass(const RenderPass&) = delete;

	RenderPass& operator=(const RenderPass&) = delete;

	RenderPass(const RenderPass&&) = delete;

	RenderPass& operator=(const RenderPass&&) = delete;

	// The window that created this render pass.
	const Window& window;

	// In our case we'll have just a single color buffer attachment represented by one of the images from the swap chain.
	VkAttachmentDescription colorAttachment{};

	// Describes the layout for the color attachment subpass.
	VkAttachmentReference colorAttachmentRef{};

	VkSubpassDescription subpass{};

	// The information used to create the render pass.
	VkRenderPassCreateInfo createInfo{};

	// The render pass.
	VkRenderPass renderPass = VK_NULL_HANDLE;
};

#endif // RENDER_PASS_H