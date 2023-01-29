#include "DepthStencilPipelineState.h"

DepthStencilPipelineState::DepthStencilPipelineState()
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	createInfo.depthTestEnable = VK_TRUE;
	createInfo.depthWriteEnable = VK_TRUE;
	createInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	createInfo.depthBoundsTestEnable = VK_FALSE;
	createInfo.minDepthBounds = 0.0f; // Optional
	createInfo.maxDepthBounds = 1.0f; // Optional
	createInfo.stencilTestEnable = VK_FALSE;
	createInfo.front = {}; // Optional
	createInfo.back = {}; // Optional
}

DepthStencilPipelineState::~DepthStencilPipelineState()
{
}

const VkPipelineDepthStencilStateCreateInfo& DepthStencilPipelineState::operator*() const
{
	return createInfo;
}
