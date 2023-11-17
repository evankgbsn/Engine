#include "Skybox.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedStaticGraphicsObject.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Images/TextureManager.h"
#include "Component/TransformComponent.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"
#include "Time/TimeManager.h"
#include "Input/InputManager.h"
#include "Engine.h"
#include "Renderer/Windows/WindowManager.h"
#include "UI/UserInterfaceItem.h"

UserInterfaceItem* uiItem = nullptr;

Skybox::Skybox()
{
	auto callback = [this](GraphicsObject* go) 
	{
		TransformComponent* transform = GetComponent<TransformComponent>(Component::Type::TRANSFORM);
		static bool scaled = false;
		if (!scaled && transform != nullptr && GraphicsObjectManager::Operating())
		{
			transform->SetScale(glm::vec3(100.0f, 100.0f, 100.0f));
			TexturedStaticGraphicsObject* ts = static_cast<TexturedStaticGraphicsObject*>(go);

			if (ts != nullptr)
			{
				ts->Scale(glm::vec3(1000.0f, 1000.0f, 1000.0f));
				scaled = true;
			}
		}

		uiItem = new UserInterfaceItem(ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("VikingRoom"));

	};

	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("Skybox"), TextureManager::GetTexture("Skybox"), callback);
}

Skybox::~Skybox()
{
	if(uiItem != nullptr)
		delete uiItem;
}

void Skybox::Update()
{
	std::function<void(const glm::vec2&)> cursorMovedCallback = [](const glm::vec2& newCursorPosition)
	{
		auto onHover = []()
		{
			if (uiItem != nullptr)
			{
				uiItem->Translate(0.2f, 0.2f);	
			}
		};
		
		if(uiItem != nullptr)
			uiItem->Hovered(onHover);
	};

	static unsigned int i = 0;
	i++;

	if(i%5 == 0)
		InputManager::WhenCursorMoved(cursorMovedCallback);
}
