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
#include "../Renderer/Images/Texture.h"

std::function<void()> UserInterfaceItem::emptyFunctionObject = std::function<void()>();

UserInterfaceItem::UserInterfaceItem(const std::string& itemName, Model* const model, Texture* const tex, float z, const glm::vec2& initialPosition, const glm::vec2& initialScale) :
	subItems(std::unordered_map<std::string, UserInterfaceItem*>()),
	name(itemName),
	whenTransformReady(std::function<void()>([]() {})),
	graphicsObject(nullptr),
	position(initialPosition),
	angle(0.0f),
	transformReady(false),
	currentVisibility(Visibility::Visible),
	graphicsObjectReadyCallbacks(std::list<std::function<void()>>()),
	ready(false),
	texture(tex),
	scale(initialScale),
	zOrder(z)
{
	std::function<void(GraphicsObject*)> graphicsObjectCreationCallback = [this](GraphicsObject* obj)
	{
		graphicsObject = static_cast<TexturedStatic2DGraphicsObject*>(obj);

		float width = UserInterfaceManager::GetWindowWidth();
		float height = UserInterfaceManager::GetWindowHeight();

		graphicsObject->TranslateObject({ (position.x > 0) ? glm::min(position.x, width) : 0.0f, (position.y > 0) ? glm::min(position.y, height) : 0.0f });
		graphicsObject->ScaleObject(scale);

		this->transformReady = true;
		whenTransformReady();

		for (std::function<void()>& callback : graphicsObjectReadyCallbacks)
		{
			callback();
		}

		graphicsObjectReadyCallbacks.clear();

		graphicsObject->SetZOrder(zOrder);

		ready = true;
	};

	GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(model, tex, graphicsObjectCreationCallback);
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
	std::function<void()> graphicsObjectReadyCallback = [this, x, y]() { graphicsObject->ScaleObject(glm::vec2(x, y)); };
	
	if (ready)
	{
		graphicsObjectReadyCallback();
	}
	else
	{
		graphicsObjectReadyCallbacks.push_back(graphicsObjectReadyCallback);
	}
}

void UserInterfaceItem::Rotate(float angle)
{
	std::function<void()> graphicsObjectReadyCallback = [this, angle]() { graphicsObject->RotateObject(angle); };
	
	if (ready)
	{
		graphicsObjectReadyCallback();
	}
	else
	{
		graphicsObjectReadyCallbacks.push_back(graphicsObjectReadyCallback);
	}
}

void UserInterfaceItem::Translate(float x, float y)
{
	std::function<void()> graphicsObjectReadyCallback = [this, x, y]() { graphicsObject->TranslateObject({ x, y }); };
	
	if (ready)
	{
		graphicsObjectReadyCallback();
	}
	else
	{
		graphicsObjectReadyCallbacks.push_back(graphicsObjectReadyCallback);
	}

}

glm::vec2 UserInterfaceItem::GetScale() const
{
	// Assumiong the graphics object has been created. Adding callbacks for these might be nessecary.
	return (graphicsObject != nullptr) ? graphicsObject->GetScale() : glm::vec2(1.0f, 1.0f);
}

void UserInterfaceItem::SetPosition(float x, float y)
{
	glm::vec2 scale = graphicsObject->GetScale();
	graphicsObject->ScaleObjectUnordered({1.0f / scale.x, 1.0f / scale.y});
	glm::vec2 translation = graphicsObject->GetTranslation();
	graphicsObject->TranslateObjectUnordered({ translation.x, translation.y });
	graphicsObject->TranslateObjectUnordered({ x, y });
	graphicsObject->ScaleObjectUnordered(scale);
	graphicsObject->SetZOrder(zOrder);
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
	if (!transformReady)
	{
		whenTransformReady = standardFunctionObject;
	}
	else
	{
		standardFunctionObject();
	}

	return transformReady;
}

UserInterfaceItem::Visibility UserInterfaceItem::InquireVisibility(UserInterfaceItem::Visibility set)
{
	// Still need to handle subitems.

	std::function<void()> inquireVisibilityCallback = [set, this]()
	{
		currentVisibility = set;

		switch (set)
		{
		case Visibility::Default:
			break;
		case Visibility::Visible:
		case Visibility::Invisible:
			GraphicsObjectManager::ToggleGraphicsObjectDraw(graphicsObject, graphicsObject->GetGraphicsObjectType());
			break;
		default:
			break;
		}
	};

	if (ready)
	{
		inquireVisibilityCallback();
	}
	else
	{
		graphicsObjectReadyCallbacks.push_back(inquireVisibilityCallback);
	}

	return currentVisibility;
}

glm::vec2 UserInterfaceItem::GetTextureDimensions() const
{
	return glm::vec2(static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()));
}

void UserInterfaceItem::SetZOrder(float newZ)
{
	graphicsObject->SetZOrder(newZ);
	zOrder = newZ;
}

float UserInterfaceItem::GetZOrder() const
{
	return graphicsObject->GetZOrder();
}
