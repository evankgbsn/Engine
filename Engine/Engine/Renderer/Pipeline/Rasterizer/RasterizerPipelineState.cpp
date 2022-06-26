#include "RasterizerPipelineState.h"

RasterizerPipelineState::RasterizerPipelineState()
{
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	createInfo.depthClampEnable = VK_FALSE; // If depthClampEnable is set to VK_TRUE, then fragments that are beyond the nearand far planes are clamped to them as opposed to discarding them.This is useful in some special cases like shadow maps.Using this requires enabling a GPU feature.
	createInfo.rasterizerDiscardEnable = VK_FALSE; // If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.

	// The polygonMode determines how fragments are generated for geometry. The following modes are available:
	// VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
	// VK_POLYGON_MODE_LINE : polygon edges are drawn as lines
	// VK_POLYGON_MODE_POINT : polygon vertices are drawn as points
	createInfo.polygonMode = VK_POLYGON_MODE_FILL;

	createInfo.lineWidth = 1.0f;
	createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

	// The rasterizer can alter the depth values by adding a constant value or biasing them based on a fragment's slope. This is sometimes used for shadow mapping, but we won't be using it
	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.depthBiasConstantFactor = 0.0f;
	createInfo.depthBiasClamp = 0.0f;
	createInfo.depthBiasSlopeFactor = 1.0f;
}

RasterizerPipelineState::~RasterizerPipelineState()
{
}

const VkPipelineRasterizationStateCreateInfo& RasterizerPipelineState::GetRasterizerState() const
{
	return createInfo;
}
