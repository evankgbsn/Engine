#include "OrientedBoundingBox.h"

#include "../Renderer/Model/Model.h"
#include "../Math/Math.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_access.hpp>


OrientedBoundingBox::OrientedBoundingBox(std::function<void(Entity*)> callback, Entity* owner) :
	CollisionVolume(callback, owner)
{
}

OrientedBoundingBox::~OrientedBoundingBox()
{
}

const glm::vec3& OrientedBoundingBox::GetCenter() const
{
	return center;
}

const glm::vec3& OrientedBoundingBox::GetMin() const
{
	return min;
}

const glm::vec3& OrientedBoundingBox::GetMax() const
{
	return max;
}

const glm::mat4& OrientedBoundingBox::GetWorld() const
{
	return worldMat;
}

void OrientedBoundingBox::ComputeData(Model* model, const glm::mat4& mat)
{

	// Update scale rotation and center.

	worldMat = mat;

	worldMin = mat * glm::vec4(min, 1.0f);
	worldMax = mat * glm::vec4(max, 1.0f);

	center = worldMin + ((worldMax - worldMin) / 2.f);

	worldWithScale = glm::scale(worldMat, glm::vec3(abs(min.x - max.x) / 2, abs(min.y - max.y) / 2, abs(min.z - max.z) / 2));

	worldMat[3] = glm::vec4(center, 1.0f);
}

bool OrientedBoundingBox::Intersect(const CollisionVolume& other) const
{
	return other.Intersect(*this);
}

bool OrientedBoundingBox::Intersect(const BoundingSphere& other) const
{
	return Math::Intersect(*this, other);
}

bool OrientedBoundingBox::Intersect(const OrientedBoundingBox& other) const
{
	return Math::Intersect(*this, other);
}

bool OrientedBoundingBox::Intersect(const AxisAlignedBoundingBox& other) const
{
	return Math::Intersect(*this, other);
}

void OrientedBoundingBox::Initialize(Model* model, const glm::mat4& mat)
{

	// Find the local min and max once on initialization.

	worldMat = mat;

	const std::vector<Vertex>& verts = model->GetVertices();

	min = verts[0].GetPosition();
	max = verts[0].GetPosition();

	for (const Vertex& x : verts)
	{
		glm::vec3 point = x.GetPosition();

		if (point.x >= max.x) max.x = point.x;
		if (point.x <= min.x) min.x = point.x;

		if (point.y >= max.y) max.y = point.y;
		if (point.y <= min.y) min.y = point.y;

		if (point.z >= max.z) max.z = point.z;
		if (point.z <= min.z) min.z = point.z;
	}
	
	center = worldMin + ((worldMax - worldMin) / 2.f);

	worldWithScale = glm::scale(worldMat, glm::vec3(abs(min.x - max.x) / 2, abs(min.y - max.y) / 2, abs(min.z - max.z) / 2));
	worldMat[3] = glm::vec4(center, 1.0f);
}

const glm::mat4& OrientedBoundingBox::GetWorldWithScale() const
{
	return worldWithScale;
}
