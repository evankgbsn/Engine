#include "System.h"

std::mutex System::systemIdIterativeMutex = std::mutex();

unsigned long System::systemIdIterative = 1;

std::unordered_map<unsigned long, System* const> System::systems = std::unordered_map<unsigned long, System* const>();

System::System(const Component::Type& systemComponentType) :
	components(std::vector<Component*>()),
	componentType(systemComponentType)
{
	std::lock_guard<std::mutex> guard(systemIdIterativeMutex);
	systemId = systemIdIterative++;
}

System::~System()
{
}

Component** const System::CreateComponent()
{
	components.push_back(nullptr);
	return &components[components.size() - 1];
}
