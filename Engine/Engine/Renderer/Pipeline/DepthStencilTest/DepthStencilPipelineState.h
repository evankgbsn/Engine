#ifndef DEPTH_STENCIL_PIPELINE_STATE_H
#define DEPTH_STENCIL_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class DepthStencilPipelineState
{

public:

	DepthStencilPipelineState();

	~DepthStencilPipelineState();

	const VkPipelineDepthStencilStateCreateInfo& operator*() const;

private:

	DepthStencilPipelineState(const DepthStencilPipelineState&) = delete;

	DepthStencilPipelineState& operator=(const DepthStencilPipelineState&) = delete;

	DepthStencilPipelineState(const DepthStencilPipelineState&&) = delete;

	DepthStencilPipelineState& operator=(const DepthStencilPipelineState&&) = delete;

	VkPipelineDepthStencilStateCreateInfo createInfo{};
	
};

#endif // DEPTH_STENCIL_PIPELINE_STATE_H