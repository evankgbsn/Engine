#include "GameObject.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"

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
}

GameObject::GameObject(Component* optionalComponents[], unsigned int componentCount)
{
	for (unsigned int i = 0; i < componentCount; i++)
	{
		AddComponent(optionalComponents[i]->GetType(), optionalComponents[i]);
	}
}
