#include "MultisamplingPipelineState.h"

MultisamplingPipelineState::MultisamplingPipelineState(const VkSampleCountFlagBits& samples)
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = samples;
	createInfo.minSampleShading = 1.0f;
	createInfo.pSampleMask = nullptr;
	createInfo.alphaToCoverageEnable = VK_FALSE;
	createInfo.alphaToOneEnable = VK_FALSE;
}

MultisamplingPipelineState::~MultisamplingPipelineState()
{
}

const VkPipelineMultisampleStateCreateInfo& MultisamplingPipelineState::operator*() const
{
	return createInfo;
}
