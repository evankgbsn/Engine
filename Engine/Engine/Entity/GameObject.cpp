#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

GameObject* const GameObject::Create(Component* optionalComponents[], unsigned int componentCount)
{
	//Memory System?
	return new GameObject();
}

GameObject::GameObject(Component* optionalComponents[], unsigned int componentCount)
{
	for (unsigned int i = 0; i < componentCount; i++)
	{
		AddComponent(optionalComponents[i]->GetType(), optionalComponents[i]);
	}
}
