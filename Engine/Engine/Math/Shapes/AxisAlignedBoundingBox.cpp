#include "AxisAlignedBoundingBox.h"

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize) :
	origin(initialOrigin),
	size(initialSize)
{
}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
}

void AxisAlignedBoundingBox::FromMinAndMax(const glm::vec3& min, const glm::vec3& max)
{
	origin = (min + max) * 0.5f;
	size = max - min * 0.5f;
}

const glm::vec3& AxisAlignedBoundingBox::GetOrigin() const
{
	return origin;
}

const glm::vec3& AxisAlignedBoundingBox::GetSize() const
{
	return size;
}

glm::vec3 AxisAlignedBoundingBox::GetMin() const
{
	glm::vec3 p1 = origin + size;
	glm::vec3 p2 = origin - size;

	return glm::vec3
	(
		fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z)
	);
}

glm::vec3 AxisAlignedBoundingBox::GetMax() const
{
	glm::vec3 p1 = origin + size;
	glm::vec3 p2 = origin - size;

	return glm::vec3
	(
		fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z)
	);
}
