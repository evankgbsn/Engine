#include "TransformSystem.h"

#include "../Component/TransformComponent.h"

TransformSystem::TransformSystem() :
    System(Component::Type::TRANSFORM),
    transformComponents(std::vector<TransformComponent>())
{
    transformComponents.reserve(10000);
}

TransformSystem::~TransformSystem()
{
}

TransformComponent* const TransformSystem::CreateComponent()
{
    return System::CreateComponent<TransformComponent>(transformComponents);
}
