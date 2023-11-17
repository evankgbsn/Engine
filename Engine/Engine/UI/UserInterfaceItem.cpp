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
#include "../Math/Math.h"

UserInterfaceItem::UserInterfaceItem(Model* const model, Texture* const texture) :
	graphicsObject(nullptr),
	position(glm::vec2(0.0f, 0.0f)),
	angle(0.0f),
	subItems(std::unordered_map<std::string, UserInterfaceItem*>())
{
	std::function<void(GraphicsObject*)> graphicsObjectCreationCallback = [this](GraphicsObject* obj)
	{
		graphicsObject = static_cast<TexturedStatic2DGraphicsObject*>(obj);

		//graphicsObject->ScaleObject({ 100.f, 100.f });
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
		projection[1][1] *= -1.0f;

		const glm::mat4& view = mainOrthoCam.GetView();

		const std::vector<Vertex>& vertices = modelToTest->GetVertices();

		static glm::vec4 triangleVerts[3] = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f)};

		if (Math::PointIn2DModel(modelToTest, view, projection, modelMat4, position, glm::vec2(window->GetWidth(), window->GetHeight())))
		{
			onHover();
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

void UserInterfaceItem::Translate(float x, float y)
{
	graphicsObject->TranslateObject({ x, y });
}
