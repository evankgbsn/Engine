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

GraphicsObjectManager* GraphicsObjectManager::instance = nullptr;

void GraphicsObjectManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new GraphicsObjectManager();
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

void GraphicsObjectManager::DrawObjects(VkCommandBuffer& buffer, GraphicsPipeline* graphicsPipeline, unsigned int imageIndex)
{
	VkDeviceSize offsets[] = { 0 };

	for (GraphicsObject* obj : instance->graphicsObjects)
	{
		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, **(graphicsPipeline->GetPipelineLayout()), 0, 1, &obj->GetDescriptorSet(imageIndex)(), 0, nullptr);
		vkCmdBindVertexBuffers(buffer, 0, 1, &obj->GetVertexBuffer()(), offsets);
		vkCmdBindIndexBuffer(buffer, obj->GetIndexBuffer()(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(buffer, static_cast<unsigned int>(obj->GetModel()->GetIndices().size()), 1, 0, 0, 0);
	}
}

GraphicsObjectManager::GraphicsObjectManager() :
	graphicsObjects(std::vector<GraphicsObject*>())
{
	MemoryManager::Initialize();
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

	DescriptorSetManager::Terminate();
	MemoryManager::Terminate();
}
