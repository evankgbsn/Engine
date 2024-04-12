#include "WireFrameRasterizerPipelineState.h"

WireFrameRasterizerPipelineState::WireFrameRasterizerPipelineState()
{
	createInfo.polygonMode = VK_POLYGON_MODE_LINE;
	createInfo.lineWidth = 1.5f;
}

WireFrameRasterizerPipelineState::~WireFrameRasterizerPipelineState()
{
}
