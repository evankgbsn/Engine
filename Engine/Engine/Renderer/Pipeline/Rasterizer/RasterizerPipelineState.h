#ifndef RASTERIZER_PIPELINE_STATE_H
#define RASTERIZER_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class RasterizerPipelineState
{

public:

	RasterizerPipelineState();

	~RasterizerPipelineState();

	const VkPipelineRasterizationStateCreateInfo& operator*() const;

protected:
	
	VkPipelineRasterizationStateCreateInfo createInfo{};

private:

	RasterizerPipelineState(const RasterizerPipelineState&) = delete;

	RasterizerPipelineState& operator=(const RasterizerPipelineState&) = delete;

	RasterizerPipelineState(const RasterizerPipelineState&&) = delete;

	RasterizerPipelineState& operator=(const RasterizerPipelineState&&) = delete;

};


#endif // RASTERIZER_PIPELINE_STATE_H