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
#include "UI/UserInterfaceManager.h"
#include "Renderer/Lights/LightManager.h"
#include "Renderer/Lights/DirectionalLight.h"


#include <glm/gtc/matrix_transform.hpp>

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

		UserInterfaceManager::CrateUserInterfaceItem(std::string("Coco"), ModelManager::GetModel("Bang"), TextureManager::GetTexture("Coco"), glm::vec2(100.0f, 600.0f));

	};

	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("Skybox"), TextureManager::GetTexture("Skybox"), callback);
}

Skybox::~Skybox()
{
}

void Skybox::Update()
{
	auto onHover = []()
	{
		UserInterfaceItem* cocoImage = UserInterfaceManager::GetUserInterfaceItem("Coco");
		if (cocoImage != nullptr)
		{
			cocoImage->Rotate(0.2f);
		}
	};

	UserInterfaceItem* cocoImage = UserInterfaceManager::GetUserInterfaceItem("Coco");
	if (cocoImage != nullptr)
		cocoImage->Hovered(onHover);

	static glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));

	DirectionalLight* const light = LightManager::GetDirectionalLight("MainDirLight");

	if (light != nullptr)
	{
		light->SetDirection(glm::vec4(light->GetDirection(), 1.0f)* rotationMatrix);
	}
}