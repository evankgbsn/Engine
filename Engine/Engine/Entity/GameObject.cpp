#include "GameObject.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "../Component/TransformComponent.h"

GameObject::GameObject()
{
	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("VikingRoom"), &graphicsObject);
}

GameObject::~GameObject()
{
}

GameObject* const GameObject::Create(Component* optionalComponents[], unsigned int componentCount)
{
	//Memory System?
	return new GameObject();
}

void GameObject::Update()
{
	TransformComponent* transformComp = GetComponent<TransformComponent>(Component::Type::TRANSFORM);
	transformComp->Translate(glm::vec3(0.00001f, 0.0f, 0.0f));
	TexturedAnimatedGraphicsObject* taGo = static_cast<TexturedAnimatedGraphicsObject*>(graphicsObject);

	if (taGo != nullptr && GraphicsObjectManager::Operating())
	{
		taGo->Translate(transformComp->GetTranslation());
	}

}

GameObject::GameObject(Component* optionalComponents[], unsigned int componentCount)
{
	for (unsigned int i = 0; i < componentCount; i++)
	{
		AddComponent(optionalComponents[i]->GetType(), optionalComponents[i]);
	}
}
