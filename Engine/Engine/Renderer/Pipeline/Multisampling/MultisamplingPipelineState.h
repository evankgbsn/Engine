#ifndef MULTISAMPLING_PIPELINE_STATE_H
#define MULTISAMPLING_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class MultisamplingPipelineState
{

public:

	MultisamplingPipelineState();

	~MultisamplingPipelineState();

	const VkPipelineMultisampleStateCreateInfo& GetMultisamplingState() const;

private:

	MultisamplingPipelineState(const MultisamplingPipelineState&) = delete;

	MultisamplingPipelineState& operator=(const MultisamplingPipelineState&) = delete;

	MultisamplingPipelineState(const MultisamplingPipelineState&&) = delete;

	MultisamplingPipelineState& operator=(const MultisamplingPipelineState&&) = delete;

	VkPipelineMultisampleStateCreateInfo createInfo{};
};

#endif // MULTISAMPLING_PIPELINE_STATE_H