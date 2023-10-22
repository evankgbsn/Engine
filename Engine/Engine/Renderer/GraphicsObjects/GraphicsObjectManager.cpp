#include "GraphicsObjectManager.h"

#include "../../Utils/Logger.h"
#include "TexturedAnimatedGraphicsObject.h"
#include "TexturedStaticGraphicsObject.h"
#include "LitTexturedStaticGraphicsObject.h"
#include "GoochGraphicsObject.h"
#include "TexturedStatic2DGraphicsObject.h"
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
#include "../Pipeline/Rasterizer/WireFrameRasterizerPipelineState.h"
#include "../Pipeline/Shaders/Shader.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"
#include "SPIRV-Reflect/spirv_reflect.h"

#include <filesystem>
#include <algorithm>
#include <execution>

GraphicsObjectManager* GraphicsObjectManager::instance = nullptr;

bool GraphicsObjectManager::shouldUpdate = false;

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
		if (IsPipelineFromShader(graphicsPipeline.first))
		{
			graphicsPipeline.second.first->CreateDescriptorSetLayout();
			graphicsPipeline.second.second = new GraphicsPipeline(*graphicsPipeline.second.first, window);
		}
	}

	for (auto& graphicsPipeline : graphicsPipelines)
	{
		if (!IsPipelineFromShader(graphicsPipeline.first))
		{
			const WireFrameRasterizerPipelineState* const wireFrameRasterizer = new WireFrameRasterizerPipelineState();
			graphicsPipeline.second.second = new GraphicsPipeline(*graphicsPipeline.second.first, *wireFrameRasterizer, window);
		}
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
							graphicsPipelines.insert(std::pair<std::string, std::pair<ShaderPipelineStage*, GraphicsPipeline*>>(std::string("WireFrame_") + fileName, std::pair<ShaderPipelineStage*, GraphicsPipeline*>(shaderPipelineStage, nullptr)));
						}

						// Create and add the shader to the ShaderPipelineStage.
						Shader* shader = new Shader(dirEntry, Renderer::GetVulkanPhysicalDevice());

						shaderPipelineStage->AddShader(shaderStage, shader);
						shaders.push_back(shader);
					}
				}
			}
		}
	}
}

void GraphicsObjectManager::CreateQueuedGraphicsObjects()
{
	for (const auto& graphicsCreateFunction : graphicsObjectCreateQueue)
	{
		graphicsCreateFunction();
	}

	graphicsObjectCreateQueue.clear();
}

void GraphicsObjectManager::ConvertPipelineForQueuedGraphicsObjects()
{
	for (const auto& graphicsConversionFunction : graphicsObjectPipelineConversionQueue)
	{
		graphicsConversionFunction();
	}

	graphicsObjectPipelineConversionQueue.clear();
}

bool GraphicsObjectManager::IsPipelineFromShader(const std::string& pipelineKey)
{
	size_t pieplineKeySize = pipelineKey.size();
	const std::string wireFrame("WireFrame_");
	const std::string pipelineKeyPrefix((pipelineKey.size() > wireFrame.size()) ? std::string(pipelineKey.begin(), pipelineKey.begin() + wireFrame.size()) : "");
	return pipelineKeyPrefix.compare(wireFrame);
}

void GraphicsObjectManager::CreateTexturedStaticGraphicsObject(Model* const model, Texture* const texture, std::function<void(GraphicsObject*)> callback)
{

	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateTexturedStaticGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueStaticMutex);

	std::function<void()> create = [model, texture, callback]()
	{
		TexturedStaticGraphicsObject* newGraphicsObject = nullptr;

		if (model != nullptr && texture != nullptr)
		{
			newGraphicsObject = new TexturedStaticGraphicsObject(model, texture);
			instance->texturedStaticGraphicsObjects.push_back(newGraphicsObject);
			callback(newGraphicsObject);
		}
	};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(Model* const model, Texture* const texture, std::function<void(GraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateAnimatedGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueAnimatedMutex);

	std::function<void()> create = [model, texture, callback]()
	{
		TexturedAnimatedGraphicsObject* newGraphicsObject = nullptr;
		
		if (model != nullptr && texture != nullptr)
		{
			newGraphicsObject = new TexturedAnimatedGraphicsObject(model, texture);
			instance->animatedTexturedGraphicsObjects.push_back(newGraphicsObject);
			callback(newGraphicsObject);
		}
	};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::CreateGoochGraphicsObject(Model* const model, Texture* const texture, std::function<void(GraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateStaticGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueGoochMutex);

	std::function<void()> create = [model, texture, callback]()
	{
		GoochGraphicsObject* newGraphicsObject = nullptr;

		if (model != nullptr)
		{
			newGraphicsObject = new GoochGraphicsObject(model, texture);
			instance->goochGraphicsObjects.push_back(newGraphicsObject);
			callback(newGraphicsObject);
		}
	};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::CreateLitTexturedStaticGraphicsObject(Model* const model, Texture* const texture, std::function<void(GraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateLitTexturedStaticGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueuelitStaticMutex);

	std::function<void()> create = [model, texture, callback]()
	{
		LitTexturedStaticGraphicsObject* newGraphicsObject = nullptr;

		if (model != nullptr)
		{
			newGraphicsObject = new LitTexturedStaticGraphicsObject(model, texture);
			instance->litTexturedStaticGraphicsObjects.push_back(newGraphicsObject);
			callback(newGraphicsObject);
		}
	};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(Model* const model, Texture* const texture, std::function<void(GraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueStatic2DMutex);

	std::function<void()> create = [model, texture, callback]()
	{
		TexturedStatic2DGraphicsObject* newGraphicsObject = nullptr;

		if (model != nullptr && texture != nullptr)
		{
			newGraphicsObject = new TexturedStatic2DGraphicsObject(model, texture);
			instance->texturedStaticGraphicsObjects.push_back(newGraphicsObject);
			callback(newGraphicsObject);
		}
	};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::WireFrame(GraphicsObject* obj, ObjectTypes::GraphicsObjectType type)
{
	std::function<void()> wireFrameCall = [obj, type]()
	{
		if (obj == nullptr)
			return;

		auto moveToWireFrame = [](std::vector<GraphicsObject*>& solidArray, std::vector<GraphicsObject*>& wireFrameArray, GraphicsObject* obj)
		{
			// queue these calls to update before draw each frame or mutex.
			for (unsigned int i = 0; i < solidArray.size(); i++)
			{
				GraphicsObject* graphicsObjectAtIndex = solidArray[i];

				if (graphicsObjectAtIndex == obj)
				{
					solidArray[i] = nullptr;
					bool placedInWireFrameArray = false;
					for (unsigned int j = 0; j < wireFrameArray.size(); j++)
					{
						if (wireFrameArray[j] == nullptr)
						{
							wireFrameArray[j] = obj;
							placedInWireFrameArray = true;
							break;
						}
					}

					if (!placedInWireFrameArray)
					{
						wireFrameArray.push_back(obj);
					}
				}
			}
		};

		switch (type)
		{
		case ObjectTypes::GraphicsObjectType::TexturedStatic:
			moveToWireFrame(instance->texturedStaticGraphicsObjects, instance->texturedStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::LitTexturedStatic:
			moveToWireFrame(instance->litTexturedStaticGraphicsObjects, instance->litTexturedStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::AnimatedTextured:
			moveToWireFrame(instance->animatedTexturedGraphicsObjects, instance->animatedTexturedGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::TexturedStatic2D:
			moveToWireFrame(instance->texturedStatic2DGraphicsObjects, instance->texturedStatic2DGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::Gooch:
			moveToWireFrame(instance->goochGraphicsObjects, instance->goochGraphicsObjectsWireFrame, obj);
			break;
		}
	};

	instance->graphicsObjectPipelineConversionQueue.push_back(wireFrameCall);
}

void GraphicsObjectManager::Solid(GraphicsObject* obj, ObjectTypes::GraphicsObjectType type)
{
	std::function<void()> solidCall = [obj, type]()
	{
		if (obj == nullptr)
			return;

		auto moveToWireFrame = [](std::vector<GraphicsObject*>& solidArray, std::vector<GraphicsObject*>& wireFrameArray, GraphicsObject* obj)
		{
			// queue these calls to update before draw each frame or mutex.
			for (unsigned int i = 0; i < solidArray.size(); i++)
			{
				GraphicsObject* graphicsObjectAtIndex = solidArray[i];

				if (graphicsObjectAtIndex == obj)
				{
					solidArray[i] = nullptr;
					bool placedInWireFrameArray = false;
					for (unsigned int j = 0; j < wireFrameArray.size(); j++)
					{
						if (wireFrameArray[j] == nullptr)
						{
							wireFrameArray[j] = obj;
							placedInWireFrameArray = true;
							break;
						}
					}

					if (!placedInWireFrameArray)
					{
						wireFrameArray.push_back(obj);
					}
				}
			}
		};

		switch (type)
		{
		case ObjectTypes::GraphicsObjectType::TexturedStatic:
			moveToWireFrame(instance->texturedStaticGraphicsObjects, instance->texturedStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::LitTexturedStatic:
			moveToWireFrame(instance->litTexturedStaticGraphicsObjects, instance->litTexturedStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::AnimatedTextured:
			moveToWireFrame(instance->animatedTexturedGraphicsObjects, instance->animatedTexturedGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::TexturedStatic2D:
			moveToWireFrame(instance->texturedStatic2DGraphicsObjects, instance->texturedStatic2DGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::Gooch:
			moveToWireFrame(instance->goochGraphicsObjects, instance->goochGraphicsObjectsWireFrame, obj);
			break;
		}
	};

	instance->graphicsObjectPipelineConversionQueue.push_back(solidCall);
}

const std::vector<GraphicsObject*>& GraphicsObjectManager::GetTexturedStaticGraphicsObjets()
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::GetStaticGraphicsObjects() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		static std::vector<GraphicsObject*> defaultReturn;
		return defaultReturn;
	}

	return instance->texturedStaticGraphicsObjects;
}

const std::vector<GraphicsObject*>& GraphicsObjectManager::GetTexturedAnimatedGraphicsObjects()
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::GetAnimatedGraphicsObjects() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		static std::vector<GraphicsObject*> defaultReturn;
		return defaultReturn;
	}

	return instance->animatedTexturedGraphicsObjects;
}

void GraphicsObjectManager::ExecutePendingCommands()
{
	if (instance == nullptr)
		return;
	
	instance->CreateQueuedGraphicsObjects();
	instance->ConvertPipelineForQueuedGraphicsObjects();
}

void GraphicsObjectManager::UpdateObjects()
{
	if (instance == nullptr)
		return;

	auto updateObjects = [](std::vector<GraphicsObject*>& goArray)
	{
		std::for_each(std::execution::par, goArray.begin(), goArray.end(),
			[](GraphicsObject* obj)
			{
				if (obj != nullptr)
					obj->Update();
			});
	};

	updateObjects(instance->texturedStatic2DGraphicsObjects);
	updateObjects(instance->texturedStatic2DGraphicsObjectsWireFrame);
	updateObjects(instance->animatedTexturedGraphicsObjects);
	updateObjects(instance->animatedTexturedGraphicsObjectsWireFrame);
	updateObjects(instance->texturedStaticGraphicsObjects);
	updateObjects(instance->texturedStaticGraphicsObjectsWireFrame);
	updateObjects(instance->goochGraphicsObjects);
	updateObjects(instance->goochGraphicsObjectsWireFrame);
	updateObjects(instance->litTexturedStaticGraphicsObjects);
	updateObjects(instance->litTexturedStaticGraphicsObjectsWireFrame);
}

void GraphicsObjectManager::DrawObjects(VkCommandBuffer& buffer, unsigned int imageIndex)
{
	if (instance == nullptr)
		return;

	auto drawObjects = [&buffer](const std::string& pipelineName, std::vector<GraphicsObject*>& objects)
	{
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **instance->graphicsPipelines.find(pipelineName)->second.second);

		// Look into command pools for threaded draw calls.
		for (GraphicsObject* obj : objects)
		{
			if (obj != nullptr)
			{
				vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **(instance->graphicsPipelines.find(pipelineName)->second.second->GetPipelineLayout()), 0, 1, &obj->GetDescriptorSet()(), 0, nullptr);
				vkCmdBindVertexBuffers(buffer, 0, 1, &obj->GetVertexBuffer()(), offsets);
				vkCmdBindIndexBuffer(buffer, obj->GetIndexBuffer()(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(buffer, static_cast<unsigned int>(obj->GetModel()->GetIndices().size()), 1, 0, 0, 0);
			}
		}
	};
	
	drawObjects(std::string("WireFrame_TexturedAnimated"), instance->animatedTexturedGraphicsObjectsWireFrame);
	drawObjects(std::string("TexturedAnimated"), instance->animatedTexturedGraphicsObjects);
	drawObjects(std::string("WireFrame_TexturedStatic"), instance->texturedStaticGraphicsObjectsWireFrame);
	drawObjects(std::string("WireFrame_TexturedStatic"), instance->texturedStatic2DGraphicsObjectsWireFrame);
	drawObjects(std::string("TexturedStatic"), instance->texturedStaticGraphicsObjects);
	drawObjects(std::string("TexturedStatic"), instance->texturedStatic2DGraphicsObjects);
	drawObjects(std::string("Gooch"), instance->goochGraphicsObjects);
	drawObjects(std::string("WireFrame_Gooch"), instance->goochGraphicsObjectsWireFrame);
	drawObjects(std::string("LitTexturedStatic"), instance->litTexturedStaticGraphicsObjects);
	drawObjects(std::string("WireFrame_LitTexturedStatic"), instance->litTexturedStaticGraphicsObjectsWireFrame);
}

const ShaderPipelineStage* const GraphicsObjectManager::GetShaderPipelineStage(const std::string& shaderName)
{
	if (instance != nullptr)
	{
		if (instance->graphicsPipelines.find(shaderName) != instance->graphicsPipelines.end())
		{
			return instance->graphicsPipelines[shaderName].first;
		}
		else
		{
			Logger::Log(std::string("Could not find shader name: ") + shaderName + std::string(" GraphicsObjectManager::GetShaderPipelineStage()"), Logger::Category::Warning);
			return nullptr;
		}
	}
	else
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::GetShaderPipelineStage() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return nullptr;
	}
}

bool GraphicsObjectManager::Operating()
{
	return shouldUpdate;
}

GraphicsObjectManager::GraphicsObjectManager(const Window& w) :
	texturedStaticGraphicsObjects(std::vector<GraphicsObject*>()),
	animatedTexturedGraphicsObjects(std::vector<GraphicsObject*>()),
	goochGraphicsObjects(std::vector<GraphicsObject*>()),
	litTexturedStaticGraphicsObjects(std::vector<GraphicsObject*>()),
	texturedStatic2DGraphicsObjects(std::vector<GraphicsObject*>()),
	window(w)
{
	DescriptorSetManager::Initialize();
	CreateDescriptorPools();
	shouldUpdate = true;
}

GraphicsObjectManager::~GraphicsObjectManager()
{
	shouldUpdate = false;

	auto deleteGraphicsObjectArray = [](std::vector<GraphicsObject*>& goArray)
	{
		for (unsigned int i = 0; i < goArray.size(); i++)
		{
			if (goArray[i] != nullptr)
			{
				delete goArray[i];
				goArray[i] = nullptr;
			}
		}
	};

	deleteGraphicsObjectArray(texturedStaticGraphicsObjects);
	deleteGraphicsObjectArray(animatedTexturedGraphicsObjects);
	deleteGraphicsObjectArray(goochGraphicsObjects);
	deleteGraphicsObjectArray(litTexturedStaticGraphicsObjects);
	deleteGraphicsObjectArray(texturedStatic2DGraphicsObjects);
	deleteGraphicsObjectArray(texturedStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectArray(animatedTexturedGraphicsObjectsWireFrame);
	deleteGraphicsObjectArray(goochGraphicsObjectsWireFrame);
	deleteGraphicsObjectArray(litTexturedStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectArray(texturedStatic2DGraphicsObjectsWireFrame);

	for (auto& graphicsPipeline : graphicsPipelines)
	{
		if (IsPipelineFromShader(graphicsPipeline.first))
		{
			delete graphicsPipeline.second.first;
		}
		delete graphicsPipeline.second.second;
	}

	DescriptorSetManager::Terminate();
}
