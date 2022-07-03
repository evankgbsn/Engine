#include "InputAssemblyPipelineState.h"

InputAssemblyPipelineState::InputAssemblyPipelineState()
{
	//VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
	//VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
	//VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
	//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
	//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two

	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	createInfo.primitiveRestartEnable = VK_FALSE;
}

InputAssemblyPipelineState::~InputAssemblyPipelineState()
{
}

const VkPipelineInputAssemblyStateCreateInfo& InputAssemblyPipelineState::operator*() const
{
	return createInfo;
}
