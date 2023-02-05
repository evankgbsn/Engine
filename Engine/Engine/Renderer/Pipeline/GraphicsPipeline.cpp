#include "GraphicsPipeline.h"

#include <stdexcept>

#include "../../Utils/Logger.h"
#include "../Renderer.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderPipelineStage.h"
#include "InputAssembly/InputAssemblyPipelineState.h"
#include "VertexInput/VertexInputPipelineState.h"
#include "Viewport/ViewportPipelineState.h"
#include "Rasterizer/RasterizerPipelineState.h"
#include "Multisampling/MultisamplingPipelineState.h"
#include "ColorBlending/ColorBlendingPipelineState.h"
#include "DynamicState/DynamicPipelineState.h"
#include "DepthStencilTest/DepthStencilPipelineState.h"
#include "RenderPass/RenderPass.h"
#include "PipelineLayout.h"
#include "../Vulkan/VulkanPhysicalDevice.h"
#include "../Pipeline/Shaders/DescriptorSet.h"
#include "../GraphicsObjects/GraphicsObjectManager.h"

static Shader* vertShader = nullptr;
static Shader* fragShader = nullptr;

GraphicsPipeline::GraphicsPipeline(const ViewportPipelineState& viewportPipelineState, const RenderPass& rp) :
	inputAssembly(new InputAssemblyPipelineState()),
	vertexInput(new VertexInputPipelineState()),
	viewport(viewportPipelineState),
	rasterizer(new RasterizerPipelineState()),
	multisampling(new MultisamplingPipelineState()),
	colorBlending(new ColorBlendingPipelineState()),
	dynamic(new DynamicPipelineState()),
	renderPass(rp),
	layout(new PipelineLayout(Renderer::GetVulkanPhysicalDevice(), GraphicsObjectManager::GetDescriptorSetLayout())),
	depthStencil(new DepthStencilPipelineState())
{
	vertShader = new Shader(std::string("../Engine/Engine/Renderer/Pipeline/Shaders/Vert.spv"), Renderer::GetVulkanPhysicalDevice());
	fragShader = new Shader(std::string("../Engine/Engine/Renderer/Pipeline/Shaders/Frag.spv"), Renderer::GetVulkanPhysicalDevice());
	shaders = new ShaderPipelineStage(&*vertShader, &*fragShader);

	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = (**shaders).data();
	createInfo.pInputAssemblyState = &**inputAssembly;
	createInfo.pVertexInputState = &**vertexInput;
	createInfo.pViewportState = &*viewport;
	createInfo.pRasterizationState = &**rasterizer;
	createInfo.pMultisampleState = &**multisampling;
	createInfo.pDepthStencilState = nullptr;
	createInfo.pColorBlendState = &**colorBlending;
	createInfo.pDynamicState = &**dynamic;
	createInfo.pDepthStencilState = &**depthStencil;
	createInfo.layout = **layout;
	createInfo.renderPass = *renderPass;
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	VkResult result = vkCreateGraphicsPipelines(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create a graphics pipeline"), Logger::Category::Error);
		throw std::runtime_error("Failed to create a graphics pipeline");
	}

	Logger::Log(std::string("Created a graphics pipeline"), Logger::Category::Success);
}

GraphicsPipeline::~GraphicsPipeline()
{
	vkDestroyPipeline(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), graphicsPipeline, nullptr);

	delete vertShader;
	delete fragShader;

	delete shaders;
	delete layout;
	delete dynamic;
	delete colorBlending;
	delete multisampling;
	delete rasterizer;
	delete vertexInput;
	delete inputAssembly;
	delete depthStencil;
}

const PipelineLayout* const GraphicsPipeline::GetPipelineLayout() const
{
	return layout;
}
