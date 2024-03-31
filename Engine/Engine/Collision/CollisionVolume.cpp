#include "CollisionVolume.h"

CollisionVolume::CollisionVolume(std::function<void(Entity* owner)> callback, Entity* owner)
{
}

CollisionVolume::~CollisionVolume()
{
}

void CollisionVolume::ComputeData(Model* model, const glm::mat4& matrix)
{
}
