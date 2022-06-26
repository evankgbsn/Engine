#include "ViewportPipelineState.h"

#include "../../Windows/Window.h"

ViewportPipelineState::ViewportPipelineState(const Window& w) :
	window(w)
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	createInfo.viewportCount = 1;
	createInfo.pViewports = &window.GetViewport();
	createInfo.scissorCount = 1;
	createInfo.pScissors = &window.GetScissor();
}

ViewportPipelineState::~ViewportPipelineState()
{
}

const VkPipelineViewportStateCreateInfo& ViewportPipelineState::GetViewportState() const
{
	return createInfo;
}
