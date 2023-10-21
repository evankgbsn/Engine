#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

class Window;
class VertexInputPipelineState;
class InputAssemblyPipelineState;
class ViewportPipelineState;
class RasterizerPipelineState;
class MultisamplingPipelineState;
class ColorBlendingPipelineState;
class DynamicPipelineState;
class RenderPass;
class PipelineLayout;
class ShaderPipelineStage;
class DepthStencilPipelineState;

class GraphicsPipeline
{

public:

	GraphicsPipeline(const ShaderPipelineStage& shaderPipelineStage, const Window& window);

	GraphicsPipeline(const ShaderPipelineStage& shaderPipelineStage, const RasterizerPipelineState& rasterizerPipelineState, const Window& window);

	~GraphicsPipeline();

	const VkPipeline& operator*() const { return graphicsPipeline; };

	const PipelineLayout* const GetPipelineLayout() const;

private:

	GraphicsPipeline() = delete;

	GraphicsPipeline(const GraphicsPipeline&) = delete;

	GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
	
	GraphicsPipeline(const GraphicsPipeline&&) = delete;

	GraphicsPipeline& operator=(const GraphicsPipeline&&) = delete;

	// The graphics pipeline.
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	// The info used to create this graphics pipeline.
	VkGraphicsPipelineCreateInfo createInfo{};

	InputAssemblyPipelineState* const inputAssembly;
	VertexInputPipelineState* const vertexInput;
	const RasterizerPipelineState* const rasterizer;
	MultisamplingPipelineState* const multisampling;
	ColorBlendingPipelineState* const colorBlending;
	DynamicPipelineState* const dynamic;
	DepthStencilPipelineState* const depthStencil;
	PipelineLayout* const layout;

	const ViewportPipelineState& viewportPipelineState;
	const ShaderPipelineStage& shaderPipelineStage;
	const RenderPass& renderPass;
};

#endif // GRAPHICS_PIPELINE_H