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
#include "../UI/UserInterfaceManager.h"
#include "../Math/Math.h"
#include "../Renderer/Windows/Window.h"
#include "../Renderer/Windows/WindowManager.h"

std::function<void()> UserInterfaceItem::emptyFunctionObject = std::function<void()>();

UserInterfaceItem::UserInterfaceItem(const std::string& itemName, Model* const model, Texture* const texture, const glm::vec2& initialPosition) :
	subItems(std::unordered_map<std::string, UserInterfaceItem*>()),
	name(itemName),
	whenTransformReady(std::function<void()>([]() {})),
	graphicsObject(nullptr),
	position(initialPosition),
	angle(0.0f),
	transformReady(false)
{
	std::function<void(GraphicsObject*)> graphicsObjectCreationCallback = [this](GraphicsObject* obj)
	{
		graphicsObject = static_cast<TexturedStatic2DGraphicsObject*>(obj);

		float width = UserInterfaceManager::GetWindowWidth();
		float height = UserInterfaceManager::GetWindowHeight();

		graphicsObject->TranslateObject({ (position.x > 0) ? glm::min(position.x, width) : 0.0f, (position.y > 0) ? glm::min(position.y, height) : 0.0f });

		this->transformReady = true;
		whenTransformReady();
	};

	GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(model, texture, graphicsObjectCreationCallback);
}

UserInterfaceItem::~UserInterfaceItem()
{
}

void UserInterfaceItem::AddSubItem(const std::string& name, UserInterfaceItem* const subItem)
{
	if (!subItems.contains(name))
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

const std::unordered_map<std::string, UserInterfaceItem*>& UserInterfaceItem::GetSubItems() const
{
	return subItems;
}

void UserInterfaceItem::Hovered(std::function<void()> onHover) const
{
	std::function<void(const glm::vec2&)> getCursorPositionCallback = [onHover, this](const glm::vec2& position)
	{
		// Tirangle intersection test with transform.
		const Model* const  modelToTest = graphicsObject->GetModel();
		glm::mat4 modelMat4 = graphicsObject->GetModelMat4();

		const Camera& mainOrthoCam = CameraManager::GetCamera(std::string("MainOrthoCamera"));

		glm::mat4 projection = mainOrthoCam.GetProjection();
		projection[1][1] *= -1.0f;

		const glm::mat4& view = mainOrthoCam.GetView();

		const std::vector<Vertex>& vertices = modelToTest->GetVertices();

		static glm::vec4 triangleVerts[3] = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f)};

		//const Window* const window = WindowManager::GetWindow("MainWindow");

		// Compute shader?
		if (Math::PointIn2DModel(modelToTest, view, projection, modelMat4, position, glm::vec2(UserInterfaceManager::GetWindowWidth(), UserInterfaceManager::GetWindowHeight())))
		{
			onHover();
		}
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

void UserInterfaceItem::OnWindowSizeUpdate()
{
	if (graphicsObject != nullptr)
	{
		float previousWidth = UserInterfaceManager::GetPreviousWindowWidth();
		float previousHeight = UserInterfaceManager::GetPreviousWindowHeight();

		float width = UserInterfaceManager::GetWindowWidth();
		float height = UserInterfaceManager::GetWindowHeight();

		position.x = Math::ChangeRange(0.0f, previousWidth, 0.0f, width, position.x);
		position.y = Math::ChangeRange(0.0f, previousHeight, 0.0f, height, position.y);


		graphicsObject->SetTranslation(position);
	}
}

const glm::vec2& UserInterfaceItem::GetTranslation() const
{
	static glm::vec2 toNotReturnATemporary(0.0f, 0.0f);
	toNotReturnATemporary = graphicsObject->GetTranslation();
	return toNotReturnATemporary;
}

bool UserInterfaceItem::TransformReady(std::function<void()>& standardFunctionObject)
{
	whenTransformReady = standardFunctionObject;
	return transformReady;
}
