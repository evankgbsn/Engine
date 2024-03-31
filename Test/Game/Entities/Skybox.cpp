#include "Skybox.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedStaticGraphicsObject.h"
#include "Renderer/GraphicsObjects/LitTexturedStaticGraphicsObject.h"
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
#include "UI/Text.h"
#include "Collision/BoundingSphere.h"


#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox() :
	skyBoxGraphicsObject(nullptr)
{
	auto callback = [this](GraphicsObject* go) 
	{
		this->skyBoxGraphicsObject = go;
		static bool scaled = false;
		if (!scaled && GraphicsObjectManager::Operating())
		{
			TexturedStaticGraphicsObject* ts = static_cast<TexturedStaticGraphicsObject*>(go);

			if (ts != nullptr)
			{
				ts->Scale(glm::vec3(1000.0f, 1000.0f, 1000.0f));
				scaled = true;
			}
		}
	};

	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("Skybox"), TextureManager::GetTexture("Skybox"), callback);

	
	

}

Skybox::~Skybox()
{
}

void Skybox::Update()
{
	if (skyBoxGraphicsObject != nullptr)
	{
		DirectionalLight* const light = LightManager::GetDirectionalLight("MainDirLight");

		static glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
		if (light != nullptr)
		{
			light->SetDirection(glm::vec4(light->GetDirection(), 1.0f) * rotationMatrix);
		}
	}
}