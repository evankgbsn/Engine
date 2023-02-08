#include "GraphicsObjectManager.h"

#include "../../Utils/Logger.h"
#include "GraphicsObject.h"
#include "../Pipeline/Shaders/DescriptorSet.h"
#include "../Pipeline/Shaders/DescriptorSetManager.h"
#include "../Windows/Window.h"
#include "../Windows/WindowManager.h"
#include "../Memory/MemoryManager.h"
#include "../Pipeline/GraphicsPipeline.h"
#include "../Memory/VertexBuffer.h"
#include "../Memory/IndexBuffer.h"
#include "../Model/Model.h"
#include "../Pipeline/PipelineLayout.h"
#include "../Pipeline/Shaders/ShaderPipelineStage.h"
#include "../Pipeline/Shaders/Shader.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

#include <filesystem>

GraphicsObjectManager* GraphicsObjectManager::instance = nullptr;

const std::string GraphicsObjectManager::shaderDirectoryName = std::string("../Engine/Engine/Renderer/Pipeline/Shaders/spv/");

void GraphicsObjectManager::Initialize(const Window& window)
{
	if (instance == nullptr)
	{
		instance = new GraphicsObjectManager(window);
		instance->CreateGraphicsPipelines();
	}
	else
	{
		Logger::LogAndThrow(std::string("Calling GraphicsObjectManager::Initialize() before GraphicsObjectManager::Terminate()."));
	}
}

void GraphicsObjectManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
	}
	else
	{
		Logger::LogAndThrow(std::string("Calling GraphicsObjectManager::Terminate() before GraphicsObjectManager::Initialize()."));
	}
}

const DescriptorSetLayout* const GraphicsObjectManager::GetDescriptorSetLayout()
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow("Calling GraphicsObjectManager::GetDescriptorSetLayout() before GraphicsObject::Initialize().");
		return nullptr;
	}

	return instance->graphicsObjects[0]->GetDescriptorSet(0).GetLayout();
}

void GraphicsObjectManager::CreateDescriptorPools()
{
	Window* const mainWindow = WindowManager::GetWindow("MainWindow");

	DescriptorSetManager::CreateDescriptorSetPool(100000, std::string("Descriptors"));
}

void GraphicsObjectManager::CreateGraphicsPipelines()
{
	LoadShaders();

	// After loading the shaders there should now be an entry for each shader file name.
	for (auto& graphicsPipeline : graphicsPipelines)
	{
		const ViewportPipelineState& viewportState = window.GetViewportPipelineState();
		const RenderPass& renderPass = window.GetRenderPass();
		graphicsPipeline.second.second = new GraphicsPipeline(viewportState, renderPass, *graphicsPipeline.second.first);
	}
}

void GraphicsObjectManager::LoadShaders()
{
	std::filesystem::path shaderDirectory = shaderDirectoryName;
	bool shaderFilePathExists = std::filesystem::exists(shaderDirectory);

	if (shaderFilePathExists)
	{
		for (auto& dirEntry : std::filesystem::directory_iterator(shaderDirectory))
		{
			if (dirEntry.is_regular_file())
			{
				if (dirEntry.path().has_extension())
				{
					static const std::string vert(".vertspv");
					static const std::string frag(".fragspv");
					static const std::string geo(".geospv");
					static const std::string tessE(".tessespv");
					static const std::string tessC(".tesscspv");

					// Get the file name without the extension.
					std::string fileName = dirEntry.path().stem().string();

					// Get the shader stage for this shader file if it is a shader file.
					std::string extension = dirEntry.path().extension().string();
					VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
					if (extension == vert)
					{
						shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
					}
					else if (extension == frag)
					{
						shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

					}
					else if (extension == geo)
					{
						shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
					}
					else if (extension == tessE)
					{
						shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
					}
					else if (extension == tessC)
					{
						shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
					}

					// Is this a shader file?
					if (shaderStage != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
					{
						// Get the current ShaderPipelineStage associated with this file name or create a new one.
						ShaderPipelineStage* shaderPipelineStage = nullptr;
						if (graphicsPipelines.find(fileName) != graphicsPipelines.end())
						{
							shaderPipelineStage = graphicsPipelines[fileName].first;
						}
						else
						{
							shaderPipelineStage = new ShaderPipelineStage();
							graphicsPipelines.insert(std::pair<std::string, std::pair<ShaderPipelineStage*, GraphicsPipeline*>>(fileName, std::pair<ShaderPipelineStage*, GraphicsPipeline*>(shaderPipelineStage, nullptr)));
						}

						// Create and add the shader to the ShaderPipelineStage.
						Shader* shader = new Shader(dirEntry.path().string(), Renderer::GetVulkanPhysicalDevice());
						shaderPipelineStage->AddShader(shaderStage, shader);
						shaders.push_back(shader);
					}
				}
			}
		}
	}
}

const GraphicsObject* const GraphicsObjectManager::GetDefault()
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::GetDefault() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return nullptr;
	}

	return instance->graphicsObjects[0];
}

GraphicsObject* const GraphicsObjectManager::CreateGraphicsObject(Model* const model)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return nullptr;
	}

	GraphicsObject* const newGraphicsObject = (model != nullptr) ? new GraphicsObject(model) : new GraphicsObject();
	instance->graphicsObjects.push_back(newGraphicsObject);

	return newGraphicsObject;
}

const std::vector<GraphicsObject*>& GraphicsObjectManager::GetGraphicsObjets()
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::GetGraphicsObjects() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		static std::vector<GraphicsObject*> defaultReturn;
		return defaultReturn;
	}

	return instance->graphicsObjects;
}

void GraphicsObjectManager::DrawObjects(VkCommandBuffer& buffer, unsigned int imageIndex)
{
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **instance->graphicsPipelines.find("Animated")->second.second);

	VkDeviceSize offsets[] = { 0 };

	for (GraphicsObject* obj : instance->graphicsObjects)
	{
		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **(instance->graphicsPipelines.find("Animated")->second.second->GetPipelineLayout()), 0, 1, &obj->GetDescriptorSet(imageIndex)(), 0, nullptr);
		vkCmdBindVertexBuffers(buffer, 0, 1, &obj->GetVertexBuffer()(), offsets);
		vkCmdBindIndexBuffer(buffer, obj->GetIndexBuffer()(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(buffer, static_cast<unsigned int>(obj->GetModel()->GetIndices().size()), 1, 0, 0, 0);
	}
}

GraphicsObjectManager::GraphicsObjectManager(const Window& w) :
	graphicsObjects(std::vector<GraphicsObject*>()),
	window(w)
{
	DescriptorSetManager::Initialize();
	CreateDescriptorPools();
	graphicsObjects.push_back(new GraphicsObject());
}

GraphicsObjectManager::~GraphicsObjectManager()
{
	for (GraphicsObject* graphicsObject : graphicsObjects)
	{
		delete graphicsObject;
	}

	for (auto& graphicsPipeline : graphicsPipelines)
	{
		delete graphicsPipeline.second.first;
		delete graphicsPipeline.second.second;
	}

	for (Shader* shader : shaders)
	{
		delete shader;
	}

	DescriptorSetManager::Terminate();
}
