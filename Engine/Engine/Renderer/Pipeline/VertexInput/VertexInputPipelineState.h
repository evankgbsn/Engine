#ifndef VERTEX_INPUT_PIPELINE_STATE_H
#define VERTEX_INPUT_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class VertexInputPipelineState
{

public:

	VertexInputPipelineState();

	~VertexInputPipelineState();

	const VkPipelineVertexInputStateCreateInfo& operator*() const;

private:

	VertexInputPipelineState(const VertexInputPipelineState&) = delete;

	VertexInputPipelineState& operator=(const VertexInputPipelineState&) = delete;

	VertexInputPipelineState(const VertexInputPipelineState&&) = delete;

	VertexInputPipelineState& operator=(const VertexInputPipelineState&&) = delete;

	VkPipelineVertexInputStateCreateInfo createInfo{};
};

#endif // VERTEX_INPUT_PIPELINE_STATE_H