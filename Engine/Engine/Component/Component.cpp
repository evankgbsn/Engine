#include "Component.h"

std::mutex Component::componentIdIterativeMutex = std::mutex();

unsigned long Component::componentIdIterative = 1;

Component::Component() :
	componentId(0),
	componentType(Type::VOID)
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
