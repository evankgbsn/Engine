#include "WireFrameRasterizerPipelineState.h"

WireFrameRasterizerPipelineState::WireFrameRasterizerPipelineState()
{
	createInfo.polygonMode = VK_POLYGON_MODE_LINE;
	createInfo.lineWidth = 4.0f;
}

WireFrameRasterizerPipelineState::~WireFrameRasterizerPipelineState()
{
}
