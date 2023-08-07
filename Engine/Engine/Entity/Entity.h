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

	virtual ~Entity();

	template<typename T>
	T* GetComponent(Component::Type componentType) const;

	void AddComponent(const Component::Type& type, Component*);

	void RemoveComponent(Component* componentToRemove);

	virtual void Update() = 0;

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

template<typename T>
inline T* Entity::GetComponent(Component::Type componentType) const
{
	for (Component* component : components)
	{
		if (component->GetType() == componentType)
		{
			return static_cast<T*>(component);
		}
	}

	return nullptr;
}
