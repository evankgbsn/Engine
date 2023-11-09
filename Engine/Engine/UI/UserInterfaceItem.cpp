#include "UserInterfaceItem.h"

#include <vector>

#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/TexturedStatic2DGraphicsObject.h"
#include "../Renderer/Model/Model.h"
#include "../Utils/Logger.h"
#include "../Input/InputManager.h"

UserInterfaceItem::UserInterfaceItem(Model* const model, Texture* const texture) :
	graphicsObject(nullptr),
	position(glm::vec2(0.0f, 0.0f)),
	angle(0.0f),
	subItems(std::unordered_map<std::string, UserInterfaceItem*>())
{
	std::function<void(GraphicsObject*)> graphicsObjectCreationCallback = [this](GraphicsObject* obj)
	{
		graphicsObject = static_cast<TexturedStatic2DGraphicsObject*>(obj);

		graphicsObject->ScaleObject({ 100.0f, 100.0f });
		graphicsObject->TranslateObject({ 0.0f, -100.0f });
	};

	GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(model, texture, graphicsObjectCreationCallback);
}

UserInterfaceItem::~UserInterfaceItem()
{
}

void UserInterfaceItem::AddSubItem(const std::string& name, UserInterfaceItem* const subItem)
{
	if (subItems.find(name) == subItems.end())
	{
		subItems[name] = subItem;
		Logger::Log(std::string(std::string("Added subitem ") + name), Logger::Category::Info);
	}
	else
	{
		Logger::Log(std::string("Could not add subitem ") + name + ". a sub item with this name already exists. UserInterfaceItem::AddSubItem", Logger::Category::Warning);
	}
}

UserInterfaceItem* UserInterfaceItem::RemoveSubItem(const std::string& name)
{
	auto foundItem = subItems.find(name);
	if (foundItem != subItems.end())
	{
		UserInterfaceItem* itemToReturn = foundItem->second;
		subItems.erase(foundItem);
		Logger::Log(std::string("Removed subitem ") + name, Logger::Category::Info);
		return itemToReturn;
	}

	Logger::Log(std::string("Failed to remove subitem ") + name + " could not find subitem. UserInterface::RemoveSubItem", Logger::Category::Warning);
	return nullptr;
}

UserInterfaceItem* const UserInterfaceItem::GetSubItem(std::string& name) const
{
	auto foundItem = subItems.find(name);
	if(foundItem != subItems.end())
		return foundItem->second;
	return nullptr;
}

void UserInterfaceItem::Hovered(const std::function<void()>& onHover) const
{
	std::function<void(const glm::vec2&)> getCursorPositionCallback = [onHover, this](const glm::vec2& position)
	{
		graphicsObject->TranslateObject({100.0f, 100.0f});

		// Tirangle intersection test with transform.
		const Model* const  modelToTest = graphicsObject->GetModel();
		const glm::mat4 modelMat4 = graphicsObject->GetModelMat4();

		// Compute shader?


	
	};

	InputManager::GetCursorPosition(getCursorPositionCallback);
}
