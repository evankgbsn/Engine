#ifndef WIRE_FRAME_RASTERIZER_PIPELINE_STATE_H
#define WIRE_FRAME_RASTERIZER_PIPELINE_STATE_H

#include "RasterizerPipelineState.h"

class WireFrameRasterizerPipelineState : public RasterizerPipelineState
{

public:

	WireFrameRasterizerPipelineState();

	~WireFrameRasterizerPipelineState();

private:

	WireFrameRasterizerPipelineState(const WireFrameRasterizerPipelineState&) = delete;

	WireFrameRasterizerPipelineState& operator=(const WireFrameRasterizerPipelineState&) = delete;

	WireFrameRasterizerPipelineState(const WireFrameRasterizerPipelineState&&) = delete;

	WireFrameRasterizerPipelineState& operator=(const WireFrameRasterizerPipelineState&&) = delete;

};

#endif // WIREFRAME_RASTERIZER_PIPELINE_STATE_H