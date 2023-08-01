#include "Entity.h"

std::mutex Entity::entityIdIterativeMutex = std::mutex();

unsigned long Entity::entityIdIterative = 1;

Entity::Entity() :
	components(std::list<Component*>()),
	entityId(0)
{
	std::lock_guard<std::mutex> guard(entityIdIterativeMutex);
	entityId = entityIdIterative++;
}

Entity::~Entity()
{
}

void Entity::AddComponent(const Component::Type& type, Component* component)
{
	components.push_back(component);
}

void Entity::RemoveComponent(Component* componentToRemove)
{
	components.remove(componentToRemove);
}
