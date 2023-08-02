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

	template<typename T>
	T* const CreateComponent(std::vector<T>& inComponentsVector);

private:
	
	Component** const CreateComponent();

	static std::mutex systemIdIterativeMutex;

	static unsigned long systemIdIterative;

	unsigned long systemId;

	System(const System&) = delete;

	System& operator=(const System&) = delete;

	System(System&&) = delete;

	System& operator=(System&&) = delete;

	Component::Type componentType = Component::Type::VOID;

	std::mutex systemCreateComponentMutex;

	std::mutex derivedClassCreateComponentMutex;

	std::vector<Component*> components;

	static std::unordered_map<unsigned long, System* const> systems;
};

#endif // System_H

template<typename T>
inline T* const System::CreateComponent(std::vector<T>& inComponentsVector)
{
	std::lock_guard<std::mutex> guard(derivedClassCreateComponentMutex);
	Component** newComponent = System::CreateComponent();
	inComponentsVector.push_back(T());
	*newComponent = &inComponentsVector[inComponentsVector.size() - 1];
	return &inComponentsVector[inComponentsVector.size() - 1];
}