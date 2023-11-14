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
		graphicsObject->TranslateObject({ -1000.0f, -500.0f });
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
		// Tirangle intersection test with transform.
		const Model* const  modelToTest = graphicsObject->GetModel();
		const glm::mat4 modelMat4 = graphicsObject->GetModelMat4();

		const std::vector<Vertex>& vertices = modelToTest->GetVertices();

		glm::vec4 triangleVerts[3] = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f)};

		for (unsigned int i = 0; i < modelToTest->GetIndices().size(); i += 3)
		{
			triangleVerts[0] = glm::vec4(vertices[modelToTest->GetIndices()[i]].GetPosition(), 1.0f) * modelMat4;
			triangleVerts[1] = glm::vec4(vertices[modelToTest->GetIndices()[i + 1]].GetPosition(), 1.0f) * modelMat4;
			triangleVerts[2] = glm::vec4(vertices[modelToTest->GetIndices()[i + 2]].GetPosition(), 1.0f) * modelMat4;

			triangleVerts[0] += modelMat4[3];
			triangleVerts[1] += modelMat4[3];
			triangleVerts[2] += modelMat4[3];

			glm::vec2 zeroToOne = triangleVerts[1] - triangleVerts[0];
			glm::vec2 zeroToTwo = triangleVerts[2] - triangleVerts[0];
			glm::vec2 oneToTwo = triangleVerts[2] - triangleVerts[1];

			glm::vec2 zeroToPosition = glm::vec4(-position, 0.0f, 1.0f) - triangleVerts[0];
			glm::vec2 oneToPosition = glm::vec4(-position, 0.0f, 1.0f) - triangleVerts[1];
			glm::vec2 twoToPosition = glm::vec4(-position, 0.0f, 1.0f) - triangleVerts[2];

			float zeroToOneLength = glm::length2(zeroToOne);
			float zeroToTwoLength = glm::length2(zeroToTwo);
			float oneToTwoLength = glm::length2(oneToTwo);

			float zeroToPositionLength = glm::length2(zeroToPosition) * 1.9f;
			float oneToPositionLength = glm::length2(oneToPosition) * 1.9f;
			float twoToPositionLength = glm::length2(twoToPosition) * 1.9f;

			float sumToPosition = zeroToPositionLength + oneToPositionLength + twoToPositionLength;
			float sumOfSides = zeroToOneLength + zeroToTwoLength + oneToTwoLength;
			
			if (sumToPosition <= sumOfSides)
			{
				onHover();
			}
		}
		// Compute shader?
	};

	InputManager::GetCursorPosition(getCursorPositionCallback);
}


void UserInterfaceItem::Scale(float x, float y)
{
	{
		graphicsObject->ScaleObject(glm::vec2(x, y));
	}
}