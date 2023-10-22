#include "Skybox.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedStaticGraphicsObject.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Images/TextureManager.h"
#include "Component/TransformComponent.h"

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

			//GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::TexturedStatic);
		}
	};

	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("Skybox"), TextureManager::GetTexture("Skybox"), callback);
}

Skybox::~Skybox()
{
}

void Skybox::Update()
{
	
}
