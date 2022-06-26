#include "VertexInputPipelineState.h"

VertexInputPipelineState::VertexInputPipelineState()
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo.vertexBindingDescriptionCount = 0;
	createInfo.pVertexBindingDescriptions = nullptr;
	createInfo.vertexAttributeDescriptionCount = 0;
	createInfo.pVertexAttributeDescriptions = nullptr;
}

VertexInputPipelineState::~VertexInputPipelineState()
{
}

const VkPipelineVertexInputStateCreateInfo& VertexInputPipelineState::Get() const
{
	return createInfo;
}
