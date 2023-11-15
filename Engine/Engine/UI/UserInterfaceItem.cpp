#include "UserInterfaceItem.h"

#include <vector>

#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/TexturedStatic2DGraphicsObject.h"
#include "../Renderer/Model/Model.h"
#include "../Utils/Logger.h"
#include "../Input/InputManager.h"
#include "../Utils/Logger.h"
#include "../Renderer/Cameras/CameraManager.h"
#include "../Renderer/Cameras/Camera.h"
#include "../Renderer/Windows/WindowManager.h"
#include "../Renderer/Windows/Window.h"

UserInterfaceItem::UserInterfaceItem(Model* const model, Texture* const texture) :
	graphicsObject(nullptr),
	position(glm::vec2(0.0f, 0.0f)),
	angle(0.0f),
	subItems(std::unordered_map<std::string, UserInterfaceItem*>())
{
	std::function<void(GraphicsObject*)> graphicsObjectCreationCallback = [this](GraphicsObject* obj)
	{
		graphicsObject = static_cast<TexturedStatic2DGraphicsObject*>(obj);

		graphicsObject->ScaleObject({ 100.f, 100.f });
		graphicsObject->TranslateObject({ 500.0f, 500.0f });
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
		glm::mat4 modelMat4 = graphicsObject->GetModelMat4();

		const Camera& mainOrthoCam = CameraManager::GetCamera(std::string("MainOrthoCamera"));

		Window* window = WindowManager::GetWindow("MainWindow");

		glm::mat4 projection = mainOrthoCam.GetProjection();
		const glm::mat4& view = mainOrthoCam.GetView();

		const std::vector<Vertex>& vertices = modelToTest->GetVertices();

		glm::vec4 triangleVerts[3] = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f)};

		for (unsigned int i = 0; i < modelToTest->GetIndices().size(); i += 3)
		{
			projection[1][1] *= -1.0f;

			triangleVerts[0] = projection * view * modelMat4 * glm::vec4(vertices[modelToTest->GetIndices()[i]].GetPosition(), 1.0f);
			triangleVerts[1] = projection * view * modelMat4 * glm::vec4(vertices[modelToTest->GetIndices()[i + 1]].GetPosition(), 1.0f);
			triangleVerts[2] = projection * view * modelMat4 * glm::vec4(vertices[modelToTest->GetIndices()[i + 2]].GetPosition(), 1.0f);

			float width = window->GetWidth();
			float height = window->GetHeight();

			glm::vec2 windowSize = glm::vec2(width, height);
			glm::vec2 windowSizeAndOffset = windowSize + glm::vec2(0.0f, 0.0f);

			double triangleVert0[2] =
			{
				(((triangleVerts[0].x / triangleVerts[0].w) + 1.0) / 2.0) * windowSizeAndOffset.x,
				(((triangleVerts[0].y / triangleVerts[0].w) + 1.0) / 2.0) * windowSizeAndOffset.y
			};

			double triangleVert1[2] =
			{
				(((triangleVerts[1].x / triangleVerts[1].w) + 1.0) / 2.0) * windowSizeAndOffset.x,
				(((triangleVerts[1].y / triangleVerts[1].w) + 1.0) / 2.0) * windowSizeAndOffset.y
			};

			double triangleVert2[2] =
			{
				(((triangleVerts[2].x / triangleVerts[2].w) + 1.0) / 2.0) * windowSizeAndOffset.x,
				(((triangleVerts[2].y / triangleVerts[2].w) + 1.0) / 2.0) * windowSizeAndOffset.y
			};

			glm::vec2 windowSpacePos0(triangleVert0[0], triangleVert0[1]);
			glm::vec2 windowSpacePos1(triangleVert1[0], triangleVert1[1]);
			glm::vec2 windowSpacePos2(triangleVert2[0], triangleVert2[1]);

			glm::vec2 zeroToOne = windowSpacePos1 - windowSpacePos0;
			glm::vec2 zeroToTwo = windowSpacePos2 - windowSpacePos0;
			glm::vec2 oneToTwo = windowSpacePos2 - windowSpacePos1;

			// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
			auto sign = [](const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) -> float
			{
				return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
			};

			auto pointInTriangle = [sign](const glm::vec2& point, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) -> bool
			{
					float d1, d2, d3;
					bool has_neg, has_pos;

					d1 = sign(point, a, b);
					d2 = sign(point, b, c);
					d3 = sign(point, c, a);

					has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
					has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

					return !(has_neg && has_pos);
			};

			if (pointInTriangle(position, windowSpacePos0, windowSpacePos1, windowSpacePos2))
			{
				onHover();
			}

			//char stringBufferx[5] = { '\0','\0','\0','\0','\0' };
			//_itoa_s(position.x, stringBufferx, 10);
			//char stringBuffery[5] = { '\0','\0','\0','\0','\0' };
			//_itoa_s(position.y, stringBuffery, 10);
			//
			//Logger::Log(std::string("Position: ") + stringBufferx + std::string(" ,") + stringBuffery, Logger::Category::Info);
		}
		// Compute shader?
	};

	InputManager::GetCursorPosition(getCursorPositionCallback);
}


void UserInterfaceItem::Scale(float x, float y)
{
	graphicsObject->ScaleObject(glm::vec2(x, y));
}

void UserInterfaceItem::Rotate(float angle)
{
	graphicsObject->RotateObject(angle);
}
