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

	template<typename T>
	std::vector<T> CreateComponentTypeContainer() const;

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

	std::vector<Component*> components;

	static std::unordered_map<unsigned long, System* const> systems;
};

#endif // System_H

template<typename T>
inline std::vector<T> System::CreateComponentTypeContainer() const
{
	return std::vector<T>();
}

template<typename T>
inline T* const System::CreateComponent(std::vector<T>& inComponentsVector)
{
	// Memory contiguous components.
	Component** newComponent = System::CreateComponent();
	*newComponent = new T();
	inComponentsVector.push_back(*(static_cast<T*>(*newComponent)));
	return &inComponentsVector[inComponentsVector.size() - 1];
}