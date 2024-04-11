#include "GraphicsObjectManager.h"

#include "../../Utils/Logger.h"
#include "TexturedAnimatedGraphicsObject.h"
#include "TexturedStaticGraphicsObject.h"
#include "LitTexturedStaticGraphicsObject.h"
#include "GoochGraphicsObject.h"
#include "TexturedStatic2DGraphicsObject.h"
#include "ColoredStaticGraphicsObject.h"
#include "ColoredAnimatedGraphicsObject.h"
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

const std::string GraphicsObjectManager::shaderDirectoryName = std::string("Assets/Shaders/");

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

void GraphicsObjectManager::DeleteQueuedGraphicsObjects()
{
	for (const auto& graphicsDeleteFunction : graphicsObjectDeleteQueue)
	{
		graphicsDeleteFunction();
	}

	graphicsObjectDeleteQueue.clear();
}

void GraphicsObjectManager::ToggleQueuedGraphicsObjects()
{
	for (const auto& toggleGraphicsObjectFunction : graphicsObjectToggleQueue)
	{
		toggleGraphicsObjectFunction();
	}

	graphicsObjectToggleQueue.clear();
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

void GraphicsObjectManager::CreateTexturedStaticGraphicsObject(const Model* const model, Texture* const texture, std::function<void(TexturedStaticGraphicsObject*)> callback)
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

void GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(const Model* const model, Texture* const texture, std::function<void(TexturedAnimatedGraphicsObject*)> callback)
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

void GraphicsObjectManager::CreateGoochGraphicsObject(const Model* const model, Texture* const texture, std::function<void(GoochGraphicsObject*)> callback)
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

void GraphicsObjectManager::CreateLitTexturedStaticGraphicsObject(const Model* const model, Texture* const texture, std::function<void(LitTexturedStaticGraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateLitTexturedStaticGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueLitStaticMutex);

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

void GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(const Model* const model, Texture* const texture, std::function<void(TexturedStatic2DGraphicsObject*)> callback)
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
			instance->texturedStatic2DGraphicsObjects.push_back(newGraphicsObject);
			callback(newGraphicsObject);
		}
	};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::CreateColoredStaticGraphicsObject(const Model* const model, const glm::vec4& color, std::function<void(ColoredStaticGraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateColoredStaticGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueColoredStaticMutex);

	std::function<void()> create = [model, callback, color]()
		{
			ColoredStaticGraphicsObject* newGraphicsObject = nullptr;

			if (model != nullptr)
			{
				newGraphicsObject = new ColoredStaticGraphicsObject(model, color);
				instance->coloredStaticGraphicsObjects.push_back(newGraphicsObject);
				callback(newGraphicsObject);
			}
		};

	instance->graphicsObjectCreateQueue.push_back(create);
}

void GraphicsObjectManager::CreateColoredAnimatedGraphicsObject(const Model* const model, const glm::vec4& color, std::function<void(ColoredAnimatedGraphicsObject*)> callback)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::CreateColoredAnimatedGraphicsObject() before GraphicsObjectManager::Initialize()."), Logger::Category::Warning);
		return;
	}

	std::lock_guard<std::mutex> guard(instance->enqueueColoredAnimatedMutex);

	std::function<void()> create = [model, callback, color]()
		{
			ColoredAnimatedGraphicsObject* newGraphicsObject = nullptr;

			if (model != nullptr)
			{
				newGraphicsObject = new ColoredAnimatedGraphicsObject(model, color);
				instance->coloredAnimatedGraphicsObjects.push_back(newGraphicsObject);
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

		auto moveToWireFrame = [](std::vector<GraphicsObject*>& solidArray, std::vector<std::pair<GraphicsObject*, unsigned int>>& wireFrameArray, GraphicsObject* obj)
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
						if (wireFrameArray[j].first == nullptr)
						{
							wireFrameArray[j].first = obj;
							wireFrameArray[j].second = i;
							placedInWireFrameArray = true;
							break;
						}
					}

					if (!placedInWireFrameArray)
					{
						wireFrameArray.push_back(std::make_pair(obj, i));
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
		case ObjectTypes::GraphicsObjectType::ColoredStatic:
			moveToWireFrame(instance->coloredStaticGraphicsObjects, instance->coloredStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::ColoredAnimated:
			moveToWireFrame(instance->coloredAnimatedGraphicsObjects, instance->coloredAnimatedGraphicsObjectsWireFrame, obj);
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

		auto moveToSolid = [](std::vector<GraphicsObject*>& solidArray, std::vector<std::pair<GraphicsObject*, unsigned int>>& wireFrameArray, GraphicsObject* obj)
		{
			for (std::pair<GraphicsObject*, unsigned int>& go : wireFrameArray)
			{
				if (go.first == obj)
				{
					solidArray[go.second] = obj;
					go.first = nullptr;
				}
			}
		};

		switch (type)
		{
		case ObjectTypes::GraphicsObjectType::TexturedStatic:
			moveToSolid(instance->texturedStaticGraphicsObjects, instance->texturedStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::LitTexturedStatic:
			moveToSolid(instance->litTexturedStaticGraphicsObjects, instance->litTexturedStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::AnimatedTextured:
			moveToSolid(instance->animatedTexturedGraphicsObjects, instance->animatedTexturedGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::TexturedStatic2D:
			moveToSolid(instance->texturedStatic2DGraphicsObjects, instance->texturedStatic2DGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::Gooch:
			moveToSolid(instance->goochGraphicsObjects, instance->goochGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::ColoredStatic:
			moveToSolid(instance->coloredStaticGraphicsObjects, instance->coloredStaticGraphicsObjectsWireFrame, obj);
			break;
		case ObjectTypes::GraphicsObjectType::ColoredAnimated:
			moveToSolid(instance->coloredAnimatedGraphicsObjects, instance->coloredAnimatedGraphicsObjectsWireFrame, obj);
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

void GraphicsObjectManager::ToggleGraphicsObjectDraw(GraphicsObject* const graphicsObjectToToggle, ObjectTypes::GraphicsObjectType type)
{
	auto findGraphicsObjectAndRemoveFromDrawVector = [](GraphicsObject* const graphicsObject, const ObjectTypes::GraphicsObjectType& type)
	{
		size_t index = 0;
		auto inCollectionLambda = [&index, graphicsObject](const std::vector<GraphicsObject*>& graphicsObjectContainer) -> bool
		{
			size_t size = graphicsObjectContainer.size();
			for (size_t i = 0; i < size; i++)
			{
				if (graphicsObjectContainer[i] == graphicsObject)
				{
					index = i;
					return true;
				}
			}

			return false;
		};

		auto inWireFrameCollectionLambda = [&index, graphicsObject](const std::vector<std::pair<GraphicsObject*, unsigned int>>& graphicsObjectContainer) -> bool
			{
				size_t size = graphicsObjectContainer.size();
				for (size_t i = 0; i < size; i++)
				{
					if (graphicsObjectContainer[i].first == graphicsObject)
					{
						index = i;
						return true;
					}
				}

				return false;
			};

		auto isDisabledLambda = [graphicsObject](std::list<std::pair<GraphicsObject*, unsigned int>>& graphicsObjectDisabledContainer, std::list<std::pair<GraphicsObject*, unsigned int>>::iterator& outIt) -> bool
		{
			for (std::list<std::pair<GraphicsObject*, unsigned int>>::iterator it = graphicsObjectDisabledContainer.begin(); it != graphicsObjectDisabledContainer.end(); it++)
			{
				if (it->first == graphicsObject)
				{
					outIt = it;
					return true;
				}
			}

			return false;
		};

		auto insertFromDisabledListAddToDrawVectorLambda = [graphicsObject](std::vector<GraphicsObject*>& drawVector, std::list<std::pair<GraphicsObject*, unsigned int>>& disableList, std::list<std::pair<GraphicsObject*, unsigned int>>::iterator graphicsObjectToRemoveFromDisableListIterator)
		{
				drawVector[graphicsObjectToRemoveFromDisableListIterator->second] = graphicsObjectToRemoveFromDisableListIterator->first;
				disableList.erase(graphicsObjectToRemoveFromDisableListIterator);
		};

		auto insertFromDisabledListAddToWireFrameDrawVectorLambda = [graphicsObject](std::vector<std::pair<GraphicsObject*, unsigned int>>& drawVector, std::list<std::pair<GraphicsObject*, unsigned int>>& disableList, std::list<std::pair<GraphicsObject*, unsigned int>>::iterator graphicsObjectToRemoveFromDisableListIterator)
			{
				drawVector[graphicsObjectToRemoveFromDisableListIterator->second].first = graphicsObjectToRemoveFromDisableListIterator->first;
				disableList.erase(graphicsObjectToRemoveFromDisableListIterator);
			};


		auto toggleGraphicsObject = [&index, inCollectionLambda, inWireFrameCollectionLambda, isDisabledLambda, insertFromDisabledListAddToDrawVectorLambda, insertFromDisabledListAddToWireFrameDrawVectorLambda](std::vector<GraphicsObject*>& graphicsObjectContainer, std::list<std::pair<GraphicsObject*, unsigned int>>& graphicsObjectDisableList, std::vector<std::pair<GraphicsObject*, unsigned int>>& graphicsObjectWireFrameContainer, std::list<std::pair<GraphicsObject*, unsigned int>>& graphicsObjectWireFrameDisableList)
		{
			std::list<std::pair<GraphicsObject*, unsigned int>>::iterator outIt;
			std::list<std::pair<GraphicsObject*, unsigned int>>::iterator outItWireframe;

			bool isDisabledGraphicsObject = isDisabledLambda(graphicsObjectDisableList, outIt);
			bool isDisabledGraphicsObjectWireFrame = isDisabledLambda(graphicsObjectWireFrameDisableList, outItWireframe);

			bool inCollectionWireFrame = inWireFrameCollectionLambda(graphicsObjectWireFrameContainer);
			bool inCollection = inCollectionLambda(graphicsObjectContainer);

			bool isWireframe = !isDisabledGraphicsObjectWireFrame && inCollectionWireFrame || isDisabledGraphicsObjectWireFrame;
			bool notWireframe = !isDisabledGraphicsObject && inCollection || isDisabledGraphicsObject;

			
			if (isWireframe && isDisabledGraphicsObjectWireFrame)
			{
				insertFromDisabledListAddToWireFrameDrawVectorLambda(graphicsObjectWireFrameContainer, graphicsObjectWireFrameDisableList, outItWireframe);
			}
			else if(notWireframe && isDisabledGraphicsObject)
			{
				insertFromDisabledListAddToDrawVectorLambda(graphicsObjectContainer, graphicsObjectDisableList, outIt);
			}
			else if (isWireframe && !isDisabledGraphicsObjectWireFrame)
			{
				graphicsObjectWireFrameDisableList.push_back(std::make_pair(graphicsObjectWireFrameContainer[index].first, static_cast<unsigned int>(index)));
				graphicsObjectWireFrameContainer[index].first = nullptr;
			}
			else if (notWireframe && !isDisabledGraphicsObject)
			{
				graphicsObjectDisableList.push_back(std::make_pair(graphicsObjectContainer[index], static_cast<unsigned int>(index)));
				graphicsObjectContainer[index] = nullptr;
			}
		};

		switch (type)
		{
		case ObjectTypes::GraphicsObjectType::TexturedStatic:
			toggleGraphicsObject(instance->texturedStaticGraphicsObjects, instance->disabledTexturedStaticGraphicsObjects, instance->texturedStaticGraphicsObjectsWireFrame, instance->disabledTexturedStaticGraphicsObjectsWireFrame);
			break;
		case ObjectTypes::GraphicsObjectType::LitTexturedStatic:
			toggleGraphicsObject(instance->litTexturedStaticGraphicsObjects, instance->disabledLitTexturedStaticGraphicsObjects, instance->litTexturedStaticGraphicsObjectsWireFrame, instance->disabledLitTexturedStaticGraphicsObjectsWireFrame);
			break;
		case ObjectTypes::GraphicsObjectType::AnimatedTextured:
			toggleGraphicsObject(instance->animatedTexturedGraphicsObjects, instance->disabledAnimatedTexturedGraphicsObjects, instance->animatedTexturedGraphicsObjectsWireFrame, instance->disabledAnimatedTexturedGraphicsObjectsWireFrame);
			break;
		case ObjectTypes::GraphicsObjectType::TexturedStatic2D:
			toggleGraphicsObject(instance->texturedStatic2DGraphicsObjects, instance->disabledTexturedStatic2DGraphicsObjects, instance->texturedStatic2DGraphicsObjectsWireFrame, instance->disabledTexturedStatic2DGraphicsObjectsWireFrame);
			break;
		case ObjectTypes::GraphicsObjectType::Gooch:
			toggleGraphicsObject(instance->goochGraphicsObjects, instance->disabledGoochGraphicsObjects, instance->goochGraphicsObjectsWireFrame, instance->disabledGoochGraphicsObjectsWireFrame);
			break;
		case ObjectTypes::GraphicsObjectType::ColoredStatic:
			toggleGraphicsObject(instance->coloredStaticGraphicsObjects, instance->disabledColoredStaticGraphicsObjects, instance->coloredStaticGraphicsObjectsWireFrame, instance->disabledColoredStaticGraphicsObjectsWireFrame);
			break;
		case ObjectTypes::GraphicsObjectType::ColoredAnimated:
			toggleGraphicsObject(instance->coloredAnimatedGraphicsObjects, instance->disabledColoredAnimatedGraphicsObjects, instance->coloredStaticGraphicsObjectsWireFrame, instance->disabledColoredAnimatedGraphicsObjectsWireFrame);
			break;
		default:
			break;
		}
	};

	std::function<void()> toggleGraphicsObject = [findGraphicsObjectAndRemoveFromDrawVector, graphicsObjectToToggle, type]()
	{
		if (instance != nullptr)
		{
			findGraphicsObjectAndRemoveFromDrawVector(graphicsObjectToToggle, type);
		}
	};

	instance->graphicsObjectToggleQueue.push_back(toggleGraphicsObject);
}

void GraphicsObjectManager::ExecutePendingCommands()
{
	if (instance == nullptr)
		return;

	instance->CreateQueuedGraphicsObjects();
	instance->ToggleQueuedGraphicsObjects();
	instance->ConvertPipelineForQueuedGraphicsObjects();
	instance->DeleteQueuedGraphicsObjects();
}

void GraphicsObjectManager::UpdateObjects()
{
	if (instance == nullptr)
		return;

	auto updateObjects = [](std::vector<GraphicsObject*>& goArray)
	{
		std::lock_guard<std::mutex> guard(instance->updateMutex);
		std::for_each(std::execution::par, goArray.begin(), goArray.end(),
			[](GraphicsObject* obj)
			{
				if (obj != nullptr)
					obj->Update();
			});
	};

	auto updateWireFrameObjects = [](std::vector<std::pair<GraphicsObject*, unsigned int>>& goArray)
		{
			std::lock_guard<std::mutex> guard(instance->updateMutex);
			std::for_each(std::execution::par, goArray.begin(), goArray.end(),
				[](std::pair<GraphicsObject*, unsigned int>& obj)
				{
					if (obj.first != nullptr)
						obj.first->Update();
				});
		};

	updateObjects(instance->texturedStatic2DGraphicsObjects);
	updateObjects(instance->animatedTexturedGraphicsObjects);
	updateObjects(instance->texturedStaticGraphicsObjects);
	updateObjects(instance->litTexturedStaticGraphicsObjects);
	updateObjects(instance->goochGraphicsObjects);
	updateObjects(instance->coloredStaticGraphicsObjects);
	updateObjects(instance->coloredAnimatedGraphicsObjects);

	updateWireFrameObjects(instance->texturedStatic2DGraphicsObjectsWireFrame);
	updateWireFrameObjects(instance->animatedTexturedGraphicsObjectsWireFrame);
	updateWireFrameObjects(instance->texturedStaticGraphicsObjectsWireFrame);
	updateWireFrameObjects(instance->goochGraphicsObjectsWireFrame);
	updateWireFrameObjects(instance->litTexturedStaticGraphicsObjectsWireFrame);
	updateWireFrameObjects(instance->coloredStaticGraphicsObjectsWireFrame);
	updateWireFrameObjects(instance->coloredAnimatedGraphicsObjectsWireFrame);
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

	auto drawWireFrameObjects = [&buffer](const std::string& pipelineName, std::vector<std::pair<GraphicsObject*, unsigned int>>& objects)
		{
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **instance->graphicsPipelines.find(pipelineName)->second.second);

			// Look into command pools for threaded draw calls.
			for (std::pair<GraphicsObject*, unsigned int>& obj : objects)
			{
				if (obj.first != nullptr)
				{
					vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **(instance->graphicsPipelines.find(pipelineName)->second.second->GetPipelineLayout()), 0, 1, &obj.first->GetDescriptorSet()(), 0, nullptr);
					vkCmdBindVertexBuffers(buffer, 0, 1, &obj.first->GetVertexBuffer()(), offsets);
					vkCmdBindIndexBuffer(buffer, obj.first->GetIndexBuffer()(), 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(buffer, static_cast<unsigned int>(obj.first->GetModel()->GetIndices().size()), 1, 0, 0, 0);
				}
			}
		};
	
	drawObjects(std::string("TexturedAnimated"), instance->animatedTexturedGraphicsObjects);
	drawObjects(std::string("TexturedStatic"), instance->texturedStaticGraphicsObjects);
	drawObjects(std::string("TexturedStatic"), instance->texturedStatic2DGraphicsObjects);
	drawObjects(std::string("Gooch"), instance->goochGraphicsObjects);
	drawObjects(std::string("LitTexturedStatic"), instance->litTexturedStaticGraphicsObjects);
	drawObjects(std::string("ColoredStatic"), instance->coloredStaticGraphicsObjects);
	drawObjects(std::string("ColoredAnimated"), instance->coloredAnimatedGraphicsObjects);
	
	drawWireFrameObjects(std::string("WireFrame_TexturedAnimated"), instance->animatedTexturedGraphicsObjectsWireFrame);
	drawWireFrameObjects(std::string("WireFrame_TexturedStatic"), instance->texturedStatic2DGraphicsObjectsWireFrame);
	drawWireFrameObjects(std::string("WireFrame_TexturedStatic"), instance->texturedStaticGraphicsObjectsWireFrame);
	drawWireFrameObjects(std::string("WireFrame_Gooch"), instance->goochGraphicsObjectsWireFrame);
	drawWireFrameObjects(std::string("WireFrame_LitTexturedStatic"), instance->litTexturedStaticGraphicsObjectsWireFrame);
	drawWireFrameObjects(std::string("WireFrame_ColoredStatic"), instance->coloredStaticGraphicsObjectsWireFrame);
	drawWireFrameObjects(std::string("WireFrame_ColoredAnimated"), instance->coloredAnimatedGraphicsObjectsWireFrame);
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

void GraphicsObjectManager::DeleteGraphicsObject(GraphicsObject* go)
{
	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling GraphicsObjectManager::DeleteGraphicsObject() before GraphicsObjectManager::Initialize()"), Logger::Category::Warning);
		return;
	}


	auto deleteObjectFromDrawVector = [go](std::mutex& queueMutex, std::vector<GraphicsObject*>& graphicsObjects)
	{
		std::lock_guard<std::mutex> guard(queueMutex);
		for (GraphicsObject*& obj : graphicsObjects)
		{
			if (obj == go)
			{
				delete obj;
				obj = nullptr;
				return;
			}
		}
	};

	auto deleteWireFrameObjectFromDrawVector = [go](std::mutex& queueMutex, std::vector<std::pair<GraphicsObject*, unsigned int>>& graphicsObjects)
		{
			std::lock_guard<std::mutex> guard(queueMutex);
			for (std::pair<GraphicsObject*, unsigned int>& obj : graphicsObjects)
			{
				if (obj.first == go)
				{
					delete obj.first;
					obj.first = nullptr;
					return;
				}
			}
		};

	auto deleteFunc = [deleteObjectFromDrawVector, deleteWireFrameObjectFromDrawVector]()
	{
		std::lock_guard<std::mutex> guard(instance->updateMutex);
		deleteObjectFromDrawVector(instance->enqueueStatic2DMutex, instance->texturedStatic2DGraphicsObjects);
		deleteWireFrameObjectFromDrawVector(instance->enqueueStatic2DMutex, instance->texturedStatic2DGraphicsObjectsWireFrame);
		deleteObjectFromDrawVector(instance->enqueueAnimatedMutex, instance->animatedTexturedGraphicsObjects);
		deleteWireFrameObjectFromDrawVector(instance->enqueueAnimatedMutex, instance->animatedTexturedGraphicsObjectsWireFrame);
		deleteObjectFromDrawVector(instance->enqueueLitStaticMutex, instance->litTexturedStaticGraphicsObjects);
		deleteWireFrameObjectFromDrawVector(instance->enqueueLitStaticMutex, instance->litTexturedStaticGraphicsObjectsWireFrame);
		deleteObjectFromDrawVector(instance->enqueueGoochMutex, instance->goochGraphicsObjects);
		deleteWireFrameObjectFromDrawVector(instance->enqueueGoochMutex, instance->goochGraphicsObjectsWireFrame);
		deleteObjectFromDrawVector(instance->enqueueColoredStaticMutex, instance->coloredStaticGraphicsObjects);
		deleteWireFrameObjectFromDrawVector(instance->enqueueColoredStaticMutex, instance->coloredStaticGraphicsObjectsWireFrame);
		deleteObjectFromDrawVector(instance->enqueueColoredAnimatedMutex, instance->coloredAnimatedGraphicsObjects);
		deleteWireFrameObjectFromDrawVector(instance->enqueueColoredAnimatedMutex, instance->coloredAnimatedGraphicsObjectsWireFrame);
	};

	instance->graphicsObjectDeleteQueue.push_back(deleteFunc);
}

GraphicsObjectManager::GraphicsObjectManager(const Window& w) :
	texturedStaticGraphicsObjects(std::vector<GraphicsObject*>()),
	disabledTexturedStaticGraphicsObjects(std::list<std::pair<GraphicsObject*, unsigned int>>()),
	texturedStaticGraphicsObjectsWireFrame(std::vector<std::pair<GraphicsObject*, unsigned int>>()),
	disabledTexturedStaticGraphicsObjectsWireFrame(std::list<std::pair<GraphicsObject*, unsigned int>>()),
	animatedTexturedGraphicsObjects(std::vector<GraphicsObject*>()),
	disabledAnimatedTexturedGraphicsObjects(std::list<std::pair<GraphicsObject*, unsigned int>>()),
	goochGraphicsObjects(std::vector<GraphicsObject*>()),
	disabledGoochGraphicsObjects(std::list<std::pair<GraphicsObject*, unsigned int>>()),
	litTexturedStaticGraphicsObjects(std::vector<GraphicsObject*>()),
	disabledLitTexturedStaticGraphicsObjects(std::list<std::pair<GraphicsObject*, unsigned int>>()),
	texturedStatic2DGraphicsObjects(std::vector<GraphicsObject*>()),
	disabledTexturedStatic2DGraphicsObjects(std::list<std::pair<GraphicsObject*, unsigned int>>()),
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
	deleteGraphicsObjectArray(coloredStaticGraphicsObjects);
	deleteGraphicsObjectArray(coloredAnimatedGraphicsObjects);

	auto deleteGraphicsObjectWireFrameArray = [](std::vector<std::pair<GraphicsObject*, unsigned int>>& goArray)
		{
			for (unsigned int i = 0; i < goArray.size(); i++)
			{
				if (goArray[i].first != nullptr)
				{
					delete goArray[i].first;
					goArray[i].first = nullptr;
				}
			}
		};

	deleteGraphicsObjectWireFrameArray(texturedStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectWireFrameArray(animatedTexturedGraphicsObjectsWireFrame);
	deleteGraphicsObjectWireFrameArray(goochGraphicsObjectsWireFrame);
	deleteGraphicsObjectWireFrameArray(litTexturedStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectWireFrameArray(texturedStatic2DGraphicsObjectsWireFrame);
	deleteGraphicsObjectWireFrameArray(coloredStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectWireFrameArray(coloredAnimatedGraphicsObjectsWireFrame);

	auto deleteGraphicsObjectList = [](std::list<std::pair<GraphicsObject*, unsigned int>>& goList)
	{
		for (std::list<std::pair<GraphicsObject*, unsigned int>>::iterator it = goList.begin(); it != goList.end(); it++)
		{
			GraphicsObject* go = it->first;
			if (go != nullptr)
			{
				delete go;
			}
		}
	};

	deleteGraphicsObjectList(disabledTexturedStaticGraphicsObjects);
	deleteGraphicsObjectList(disabledAnimatedTexturedGraphicsObjects);
	deleteGraphicsObjectList(disabledGoochGraphicsObjects);
	deleteGraphicsObjectList(disabledLitTexturedStaticGraphicsObjects);
	deleteGraphicsObjectList(disabledTexturedStatic2DGraphicsObjects);
	deleteGraphicsObjectList(disabledColoredStaticGraphicsObjects);
	deleteGraphicsObjectList(disabledColoredAnimatedGraphicsObjects);
	deleteGraphicsObjectList(disabledTexturedStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectList(disabledAnimatedTexturedGraphicsObjectsWireFrame);
	deleteGraphicsObjectList(disabledGoochGraphicsObjectsWireFrame);
	deleteGraphicsObjectList(disabledLitTexturedStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectList(disabledTexturedStatic2DGraphicsObjectsWireFrame);
	deleteGraphicsObjectList(disabledColoredStaticGraphicsObjectsWireFrame);
	deleteGraphicsObjectList(disabledColoredAnimatedGraphicsObjectsWireFrame);

	for (auto& graphicsPipeline : graphicsPipelines)
	{
		if (IsPipelineFromShader(graphicsPipeline.first))
		{
			delete graphicsPipeline.second.first;
		}
		delete graphicsPipeline.second.second;
	}

	DescriptorSetManager::Terminate();

	instance = nullptr;
}
