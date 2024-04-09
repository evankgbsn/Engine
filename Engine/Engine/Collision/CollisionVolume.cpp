#include "CollisionVolume.h"

#include "../Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"

CollisionVolume::CollisionVolume(std::function<void(Entity* owner)> callback, Entity* owner)
{
}

CollisionVolume::~CollisionVolume()
{
}

const glm::vec3& CollisionVolume::GetCenter() const
{
	static glm::vec3 defaultReturn;
	return defaultReturn;
}

void CollisionVolume::ComputeData(const std::vector<Vertex>& vertices, const glm::mat4& matrix)
{
}

void CollisionVolume::Collide() const
{
	collisionCallback(owningEntity);
}

void CollisionVolume::SetColor(const glm::vec4& newColor)
{
	if (graphics != nullptr)
	{
		graphics->SetColor(newColor);
	}
}

const glm::vec4& CollisionVolume::GetColor() const
{
	static glm::vec4 defaultReturn(1.0f);

	if (graphics != nullptr)
	{
		return graphics->GetColor();
	}

	return defaultReturn;
}

void CollisionVolume::UpdateGraphicsTransform(const glm::mat4& newTransform)
{
	if (graphics != nullptr)
	{
		graphics->SetTransform(newTransform);
	}
}
