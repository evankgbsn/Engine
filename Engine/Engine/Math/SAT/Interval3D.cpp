#include "Interval3D.h"

#include "../Shapes/AxisAlignedBoundingBox.h"
#include "../Shapes/OrientedBoundingBox.h"

Interval3D::Interval3D(const AxisAlignedBoundingBox& aabb, const glm::vec3& axis)
{
	const glm::vec3 i = aabb.GetMin();
	const glm::vec3 a = aabb.GetMax();

	glm::vec3 vertex[8] =
	{
		glm::vec3(i.x, a.y, a.z),
		glm::vec3(i.x, a.y, i.z),
		glm::vec3(i.x, i.y, a.z),
		i,
		a,
		glm::vec3(a.x, a.y, i.z),
		glm::vec3(a.x, i.y, a.z),
		glm::vec3(a.x, i.y, i.z)
	};

	min = max = glm::dot(axis, vertex[0]);

	for (unsigned int i = 1; i < 8; ++i)
	{
		float projection = glm::dot(axis, vertex[i]);
		min = (projection < min) ? projection : min;
		max = (projection > max) ? projection : max;
	}
}

Interval3D::Interval3D(const OrientedBoundingBox& obb, const glm::vec3& axis)
{

	glm::vec3 vertex[8];

	const glm::vec3& center = obb.GetOrigin();
	const glm::vec3& extents = obb.GetSize();
	const glm::mat4& orientation = obb.GetOrientation();

	vertex[0] = center + glm::vec3(orientation[0]) * extents[0] + glm::vec3(orientation[1]) * extents[1] + glm::vec3(orientation[2]) * extents[2];
	vertex[1] = center - glm::vec3(orientation[0]) * extents[0] + glm::vec3(orientation[1]) * extents[1] + glm::vec3(orientation[2]) * extents[2];
	vertex[2] = center + glm::vec3(orientation[0]) * extents[0] - glm::vec3(orientation[1]) * extents[1] + glm::vec3(orientation[2]) * extents[2];
	vertex[3] = center + glm::vec3(orientation[0]) * extents[0] + glm::vec3(orientation[1]) * extents[1] - glm::vec3(orientation[2]) * extents[2];
	vertex[4] = center - glm::vec3(orientation[0]) * extents[0] - glm::vec3(orientation[1]) * extents[1] - glm::vec3(orientation[2]) * extents[2];
	vertex[5] = center + glm::vec3(orientation[0]) * extents[0] - glm::vec3(orientation[1]) * extents[1] - glm::vec3(orientation[2]) * extents[2];
	vertex[6] = center - glm::vec3(orientation[0]) * extents[0] + glm::vec3(orientation[1]) * extents[1] - glm::vec3(orientation[2]) * extents[2];
	vertex[7] = center - glm::vec3(orientation[0]) * extents[0] - glm::vec3(orientation[1]) * extents[1] + glm::vec3(orientation[2]) * extents[2];

	min = max = glm::dot(axis, vertex[0]);
	for (unsigned int i = 1; i < 8; ++i)
	{
		float projection = glm::dot(axis, vertex[i]);
		min = (projection < min) ? projection : min;
		max = (projection > max) ? projection : max;
	}
	
}

Interval3D::~Interval3D()
{
}

float Interval3D::GetMin() const
{
	return min;
}

float Interval3D::GetMax() const
{
	return max;
}
