#include "Sphere.h"

#include "AxisAlignedBoundingBox.h"
#include "OrientedBoundingBox.h"
#include "Plane.h"

Sphere::Sphere(const glm::vec3& initialOrigin, float initialRadius) :
	origin(initialOrigin),
	radius(initialRadius)
{
}

Sphere::~Sphere()
{
}

const glm::vec3& Sphere::GetOrigin() const
{
	return origin;
}

float Sphere::GetRadius() const
{
	return radius;
}

bool Sphere::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 toPoint = point - origin;
	return glm::dot(toPoint, toPoint) <= radius * radius;
}

bool Sphere::SphereIntersect(const Sphere& other) const
{
	glm::vec3 distance = other.origin - origin;

	float lengthSq = glm::dot(distance, distance);

	float sumRadii = other.radius + radius;

	return lengthSq <= sumRadii * sumRadii;
}

bool Sphere::AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const
{
	glm::vec3 closest = aabb.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

bool Sphere::OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const
{
	glm::vec3 closest = obb.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

bool Sphere::PlaneIntersect(const Plane& plane) const
{
	glm::vec3 closest = plane.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

glm::vec3 Sphere::ClosestPoint(const glm::vec3& point) const
{
	glm::vec3 toPoint = glm::normalize(point - origin);
	return origin + toPoint * radius;
}
