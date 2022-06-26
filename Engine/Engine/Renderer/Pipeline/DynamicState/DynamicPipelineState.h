#ifndef DYNAIMC_PIPELINE_STATE_H
#define DYNAMIC_PIPELINE_STATE_H

#include <vector>

#include <vulkan/vulkan.h>

class DynamicPipelineState
{

public:

	DynamicPipelineState();

	~DynamicPipelineState();

	const VkPipelineDynamicStateCreateInfo& GetDynamicState() const;

private:

	DynamicPipelineState(const DynamicPipelineState&) = delete;

	DynamicPipelineState& operator=(const DynamicPipelineState&) = delete;

	DynamicPipelineState(const DynamicPipelineState&&) = delete;

	DynamicPipelineState& operator=(const DynamicPipelineState&&) = delete;

	VkPipelineDynamicStateCreateInfo createInfo{};

	std::vector<VkDynamicState> dynamicStates = std::vector<VkDynamicState>();
};


#endif // DYNAMIC_PIPELINE_STATE_H