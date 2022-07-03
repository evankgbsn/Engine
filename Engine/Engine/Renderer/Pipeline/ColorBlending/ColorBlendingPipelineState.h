#ifndef COLOR_BLENDING_PIPELINE_STATE_H
#define COLOR_BLENDING_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class ColorBlendingPipelineState
{

public:

	ColorBlendingPipelineState();

	~ColorBlendingPipelineState();

	const VkPipelineColorBlendStateCreateInfo& operator*() const;

private:

	ColorBlendingPipelineState(const ColorBlendingPipelineState&) = delete;

	ColorBlendingPipelineState& operator=(const ColorBlendingPipelineState&) = delete;

	ColorBlendingPipelineState(const ColorBlendingPipelineState&&) = delete;

	ColorBlendingPipelineState& operator=(const ColorBlendingPipelineState&&) = delete;

	VkPipelineColorBlendStateCreateInfo createInfo{};

	// This is per frame buffer.
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
};

#endif // COLOR_BLENDING_PIPELINE_STATE_H