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

		glm::vec2 anchor;
		glm::vec2 vecOne;
		glm::vec2 vecTwo;
		glm::vec2 vecAnchorToPoint;
		glm::vec2 vecOneToPoint;
		glm::vec2 vecTwoToPoint;

		glm::vec2* vecsToPoint[3] = {&vecAnchorToPoint, &vecOneToPoint, &vecTwoToPoint};

		static const glm::vec2 vecRight(1.0f, 0.0f);

		Vertex triangleVerts[3] = { Vertex(), Vertex(), Vertex() };

		float dotVecOne = 0.0f;
		float dotVecTwo = 0.0f;
		float lengthVecOne = 0.0f;
		float lengthVecTwo = 0.0f;
		float lengthVecToPoint = 0.0f;
		float dotVecRight = 0.0f;

		bool between = false;

		glm::vec2 worldSpacePointOne(0.0f);
		glm::vec2 worldSpacePointTwo(0.0f);
		glm::vec2 worldSpacePointThree(0.0f);

		for (unsigned int i = 0; i < modelToTest->GetIndices().size(); i += 3)
		{
			triangleVerts[0] = vertices[modelToTest->GetIndices()[i]];
			triangleVerts[1] = vertices[modelToTest->GetIndices()[i + 1]];
			triangleVerts[2] = vertices[modelToTest->GetIndices()[i + 2]];

			worldSpacePointOne = glm::vec2(glm::vec4(triangleVerts[0].GetPosition(), 1.0f) * modelMat4);
			worldSpacePointTwo = glm::vec2(glm::vec4(triangleVerts[1].GetPosition(), 1.0f) * modelMat4);
			worldSpacePointThree = glm::vec2(glm::vec4(triangleVerts[2].GetPosition(), 1.0f) * modelMat4);

			worldSpacePointOne = (worldSpacePointOne + glm::vec2(modelMat4[3]));
			worldSpacePointTwo = (worldSpacePointTwo + glm::vec2(modelMat4[3]));
			worldSpacePointThree = (worldSpacePointThree + glm::vec2(modelMat4[3]));

			anchor = worldSpacePointOne;
			vecOne = worldSpacePointTwo - anchor;
			vecTwo = worldSpacePointThree - anchor;
			vecAnchorToPoint = -position - anchor;
			vecOneToPoint = -position - worldSpacePointOne;
			vecTwoToPoint = -position - worldSpacePointTwo;

			//____\|/____

			dotVecOne = glm::dot(vecOne, vecAnchorToPoint);
			dotVecTwo = glm::dot(vecTwo, vecAnchorToPoint);
			dotVecRight = glm::dot(vecRight, vecAnchorToPoint);

			bool rightOfVecOne = dotVecOne > 0 && dotVecRight > 0;
			bool leftOfVecTwo = dotVecTwo < 0 && dotVecRight < 0;

			between = rightOfVecOne && leftOfVecTwo || !rightOfVecOne && !leftOfVecTwo;
			
			if (between)
			{
				lengthVecOne = glm::length(vecOne);
				lengthVecTwo = glm::length(vecTwo);

				float lengthOposite = glm::length(worldSpacePointThree - worldSpacePointTwo);
				bool inside = true;
				for (unsigned int i = 0; i < 3; i++)
				{
					if(!(glm::length(*(vecsToPoint[i])) > lengthOposite) || !(glm::length(*(vecsToPoint[i])) > lengthVecOne) || !(glm::length(*(vecsToPoint[i])) > lengthVecTwo))
					{
						continue;
					}
					else
					{
						inside = false;
						break;
					}
				}

				if (inside)
				{
					onHover();
				}
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