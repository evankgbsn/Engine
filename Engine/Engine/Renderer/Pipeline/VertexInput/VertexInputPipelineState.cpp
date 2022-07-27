#include "VertexInputPipelineState.h"

#include "../../Models/Vertex.h"

VertexInputPipelineState::VertexInputPipelineState()
{
	auto& bindingDescription = Vertex::GetBindingDescription();
	auto& attributeDescriptions = Vertex::GetAttributeDescriptions();

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo.vertexBindingDescriptionCount = 1;
	createInfo.pVertexBindingDescriptions = &bindingDescription;
	createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	createInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
}

VertexInputPipelineState::~VertexInputPipelineState()
{
}

const VkPipelineVertexInputStateCreateInfo& VertexInputPipelineState::operator*() const
{
	return createInfo;
}
