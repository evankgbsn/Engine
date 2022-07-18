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

class GraphicsPipeline
{

public:

	GraphicsPipeline(const Window& window);

	~GraphicsPipeline();

	const VkPipeline& operator*() const { return graphicsPipeline; };

	RenderPass* const GetRenderPass() const { return renderPass; };

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

	// The window that was used when creating this pipeline.
	const Window& window;

	InputAssemblyPipelineState* const inputAssembly;
	VertexInputPipelineState* const vertexInput;
	ViewportPipelineState* const viewport;
	RasterizerPipelineState* const rasterizer;
	MultisamplingPipelineState* const multisampling;
	ColorBlendingPipelineState* const colorBlending;
	DynamicPipelineState* const dynamic;
	ShaderPipelineStage* shaders;

	PipelineLayout* const layout;

	RenderPass* const renderPass;
};

#endif // GRAPHICS_PIPELINE_H