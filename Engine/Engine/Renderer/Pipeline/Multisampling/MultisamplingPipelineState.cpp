#include "MultisamplingPipelineState.h"

MultisamplingPipelineState::MultisamplingPipelineState()
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
