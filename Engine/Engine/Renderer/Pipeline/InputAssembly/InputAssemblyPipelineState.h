#ifndef INPUT_ASSEMBLY_PIPELINE_STATE_H
#define INPUT_ASSEMBLY_PIPELINE_STATE_H

#include <vulkan/vulkan.h>

class InputAssemblyPipelineState
{

public:

	InputAssemblyPipelineState();

	~InputAssemblyPipelineState();

	const VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyState() const;

private:

	InputAssemblyPipelineState(const InputAssemblyPipelineState&) = delete;

	InputAssemblyPipelineState& operator=(const InputAssemblyPipelineState&) = delete;

	InputAssemblyPipelineState(const InputAssemblyPipelineState&&) = delete;

	InputAssemblyPipelineState& operator=(const InputAssemblyPipelineState&&) = delete;

	VkPipelineInputAssemblyStateCreateInfo createInfo{};
};


#endif // INPUT_ASSEMBLY_PIPELINE_STATE_H