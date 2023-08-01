#ifndef System_H
#define System_H

#include "../Component/Component.h"

#include <unordered_map>
#include <vector>

class System
{
public:
	
	System(const Component::Type& systemComponentType);

	~System();

protected:

	System() = default;

	virtual Component* const CreateComponent() = 0;

private:

	static std::mutex systemIdIterativeMutex;

	static unsigned long systemIdIterative;

	unsigned long systemId;

	System(const System&) = delete;

	System& operator=(const System&) = delete;

	System(System&&) = delete;

	System& operator=(System&&) = delete;

	Component::Type componentType = Component::Type::VOID;

	std::vector<Component*> components;

	static std::unordered_map<unsigned long, System* const> systems;
};

#endif // System_H