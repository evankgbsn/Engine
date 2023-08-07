#include "Component.h"

std::mutex Component::componentIdIterativeMutex = std::mutex();

unsigned long Component::componentIdIterative = 1;

Component::Component(Type type) :
	componentId(0),
	componentType(type),
	commandQueueMutex()
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

void Component::Operate()
{
	std::lock_guard<std::mutex> guard(commandQueueMutex);
	for (const auto& command : commands)
	{
		command();
	}

	commands.clear();
}

void Component::QueueCommand(const std::function<void()>& command)
{
	std::lock_guard<std::mutex> guard(commandQueueMutex);
	commands.push_back(command);
}
