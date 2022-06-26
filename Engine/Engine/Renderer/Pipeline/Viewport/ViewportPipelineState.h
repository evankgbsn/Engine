#ifndef VIEWPORT_PIPELINE_STATE_H
#define VIEWPORT_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class Window;

class ViewportPipelineState
{

public:

	ViewportPipelineState(const Window& window);

	~ViewportPipelineState();

	const VkPipelineViewportStateCreateInfo& GetViewportState() const;

private:

	ViewportPipelineState() = delete;

	ViewportPipelineState(const ViewportPipelineState&) = delete;

	ViewportPipelineState* operator=(const ViewportPipelineState&) = delete;

	ViewportPipelineState(const ViewportPipelineState&&) = delete;

	ViewportPipelineState* operator=(const ViewportPipelineState&&) = delete;

	VkPipelineViewportStateCreateInfo createInfo{};

	const Window& window;
};

#endif // VIEWPORT_PIEPLINE_STATE_H