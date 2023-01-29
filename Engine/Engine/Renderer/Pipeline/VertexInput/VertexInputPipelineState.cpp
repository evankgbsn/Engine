#include "VertexInputPipelineState.h"

#include "../../Model/Vertex.h"

VertexInputPipelineState::VertexInputPipelineState()
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	createInfo.vertexBindingDescriptionCount = 1;
	createInfo.pVertexBindingDescriptions = &Vertex::GetVertexInputBindingDescription();

	std::vector<VkVertexInputAttributeDescription>& attributeDescriptions = Vertex::GetVertexInputAttributeDescriptions();

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
