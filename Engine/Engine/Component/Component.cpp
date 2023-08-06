#include "Component.h"

std::mutex Component::componentIdIterativeMutex = std::mutex();

unsigned long Component::componentIdIterative = 1;

Component::Component(Type type) :
	componentId(0),
	componentType(type)
{
	std::lock_guard<std::mutex> guard(componentIdIterativeMutex);
	componentId = componentIdIterative++;
}

Component::~Component()
{
}

Component::Type Component::GetType() const
{
	return componentType;
}
