#ifndef Entity_H
#define Entity_H

#include "../Component/Component.h"

#include <list>

/**
	Wrapper class for Engine Entities.
*/
class Entity
{
public:

	Entity();

	~Entity();

	void AddComponent(const Component::Type& type, Component*);

	void RemoveComponent(Component* componentToRemove);

protected:

	static std::mutex entityIdIterativeMutex;

	static unsigned long entityIdIterative;

	unsigned long entityId;

	/**
		Components are not owned by the entity.
	*/
	std::list<Component*> components;

private:

	Entity(const Entity&) = delete;

	Entity& operator=(const Entity&) = delete;

	Entity(Entity&&) = delete;

	Entity& operator=(Entity&&) = delete;

};

#endif // Entity_H
