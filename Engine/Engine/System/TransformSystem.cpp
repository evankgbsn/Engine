#include "TransformSystem.h"

#include "../Component/TransformComponent.h"

TransformSystem::TransformSystem() :
    System(),
    transformComponents(std::vector<TransformComponent>())
{
}

TransformSystem::~TransformSystem()
{
}

TransformComponent* const TransformSystem::CreateComponent()
{
    return System::CreateComponent<TransformComponent>(transformComponents);
}
