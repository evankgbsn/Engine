#include "DynamicPipelineState.h"

DynamicPipelineState::DynamicPipelineState()
{
	dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
	
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	createInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	createInfo.pDynamicStates = dynamicStates.data();
}

DynamicPipelineState::~DynamicPipelineState()
{
}

const VkPipelineDynamicStateCreateInfo& DynamicPipelineState::operator*() const
{
	return createInfo;
}
