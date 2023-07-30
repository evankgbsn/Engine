#ifndef Entity_H
#define Entity_H

#include <vector>

class Component;

/**
	Wrapper class for Engine Entities.
*/
class Entity
{
public:

	Entity();

	~Entity();

protected:

	/**
		Components are not owned by the entity.
	*/
	std::vector<Component*> components;

private:

	Entity(const Entity&) = delete;

	Entity& operator=(const Entity&) = delete;

	Entity(Entity&&) = delete;

	Entity& operator=(Entity&&) = delete;

};

#endif // Entity_H
